#include "SrtTranslator.h"
#include <QDebug>
#include <QStringList>
#include <QCoreApplication>
#include <QTime>


SrtTranslator::SrtTranslator()
    : m_openaiManager(OpenaiManager::getInstance())
    , m_translatePrompt(SrtPrompt::getInstance())
{
    qDebug() << "初始化SRT文件翻译";
    m_translatePrompt->load(m_translatePrompt->getPromptPath());
}

SrtTranslator::~SrtTranslator()
{
}

bool SrtTranslator::translate(const QString& inputFilePath, 
                              const QString& outputFilePath,
                            const QString& url, 
                            const QString& apiKey,
                            const QString& model,
                            bool keepHistory)
{
    QTime startTime = QTime::currentTime();
    outlog("=== 开始翻译任务 ===");
    outlog("字幕文件: " + inputFilePath);
    outlog("翻译模型: " + model);
    outlog("");
    
    // 解析SRT文件
    if (!m_srtParser.parse(inputFilePath)) {
        outlog("[错误] 解析SRT文件失败");
        return false;
    }
    
    // 获取字幕列表
    QVector<SrtInfo> srtInfoList = m_srtParser.getSrtInfoList();
    outlog(QString("共找到 %1 条字幕").arg(srtInfoList.size()));
    outlog("-------------------");
    
    // 获取翻译提示信息
    PromptInfo promptInfo = m_translatePrompt->getPrompt();

    // 用于计算平均处理时间
    QVector<int> processingTimes;
    
    // 遍历字幕列表进行翻译
    for (int i = 0; i < srtInfoList.size(); ++i) {
        QTime itemStartTime = QTime::currentTime();
        
        // 处理用户提示列表
        QVector<QString> processedUserPrompts;
        QVector<QString> processedAssistantPrompts;

        // 先加入历史记录
        for (const TranslateHistory& history : m_history) {
            processedUserPrompts.append(history.original);
            processedAssistantPrompts.append(history.translated);
        }

        // 然后增加参考示例
        for (const QString& userPrompt : promptInfo.userPromptList) {
            QString processedPrompt = buildPrompt(userPrompt, promptInfo);
            processedUserPrompts.append(processedPrompt);
        }

        for (const QString& assistantPrompt : promptInfo.assistantPromptList) {
            processedAssistantPrompts.append(assistantPrompt);
        }

        // 限制原文显示长度，超过时截断并显示省略号
        QString displayContent = srtInfoList[i].content;
        const int maxLengthIn = 64;
        if (displayContent.length() > maxLengthIn) {
            displayContent = displayContent.left(maxLengthIn) + "...";
        }
        outlog(QString("\n[进度 %1/%2]").arg(i + 1).arg(srtInfoList.size()));
        outlog("原文: " + displayContent);
        
        // 构建当前字幕的翻译提示
        QString currentPrompt = buildPrompt(srtInfoList[i].content, promptInfo);
        
        // 创建新的用户提示列表，包含所有处理过的示例和当前字幕
        QVector<QString> finalUserPrompts = processedUserPrompts;
        finalUserPrompts.append(currentPrompt);
        
        // 调用OpenAI API进行翻译
        QString translation = m_openaiManager->sendMessage(
            url,
            apiKey,
            model,
            promptInfo.prompt,
            finalUserPrompts,
            processedAssistantPrompts
        );
        
        if (translation.isEmpty()) {
            outlog("[错误] 翻译失败，跳过当前字幕");
            outlog("-------------------");
            continue;
        }
        
        // 处理翻译结果，寻找第一个有效的内容行
        QStringList lines = translation.split('\n');
        QString validTranslation;
        
        for (const QString& line : lines) {
            QString trimmedLine = line.trimmed();
            if (!trimmedLine.isEmpty()) {
                validTranslation = trimmedLine;
                validTranslation.remove('\n');
                validTranslation.replace("\\n", " ");
                
                // 限制翻译结果长度，超过时截断
                const int maxLengthOut = 64;
                if (validTranslation.length() > maxLengthOut) {
                    validTranslation = validTranslation.left(maxLengthOut);
                }
                
                outlog("译文: " + validTranslation);
                break;
            }
        }
        
        // 保存翻译结果
        if (!validTranslation.isEmpty()) {
            srtInfoList[i].translateContent = validTranslation;
            if (keepHistory) {
                TranslateHistory history;
                history.original = currentPrompt;
                history.translated = validTranslation;
                m_history.append(history);
                
                // 保持历史记录不超过
                const int maxHistory = 5;
                while (m_history.size() > maxHistory) {
                    m_history.removeFirst();
                }
            }
        } else {
            outlog("[警告] 未找到有效的翻译内容，使用原文");
            srtInfoList[i].translateContent = srtInfoList[i].content;
        }
        
        // 计算本条字幕的处理时间
        int processTime = itemStartTime.msecsTo(QTime::currentTime());
        processingTimes.append(processTime);
        
        // 计算平均处理时间和预计剩余时间
        if (processingTimes.size() > 0) {
            // 计算平均处理时间（毫秒）
            qint64 avgTime = 0;
            for (int time : processingTimes) {
                avgTime += time;
            }
            avgTime /= processingTimes.size();
            
            // 计算预计剩余时间
            int remainingItems = srtInfoList.size() - (i + 1);
            qint64 estimatedRemainingTime = avgTime * remainingItems;
            
            // 转换为更友好的时间格式
            QString timeStr;
            if (estimatedRemainingTime < 1000) {  // 小于1秒
                timeStr = QString("%1毫秒").arg(estimatedRemainingTime);
            } else if (estimatedRemainingTime < 60000) {  // 小于1分钟
                timeStr = QString("%1秒").arg(estimatedRemainingTime / 1000);
            } else if (estimatedRemainingTime < 3600000) {  // 小于1小时
                int minutes = estimatedRemainingTime / 60000;
                int seconds = (estimatedRemainingTime % 60000) / 1000;
                timeStr = QString("%1分%2秒").arg(minutes).arg(seconds);
            } else {  // 大于1小时
                int hours = estimatedRemainingTime / 3600000;
                int minutes = (estimatedRemainingTime % 3600000) / 60000;
                timeStr = QString("%1小时%2分").arg(hours).arg(minutes);
            }
            
            outlog(QString("平均处理时间: %1ms").arg(avgTime));
            outlog(QString("预计剩余时间: %1").arg(timeStr));
        }
        
        outlog("-------------------");
    }
    
    if (!m_srtParser.save(outputFilePath, srtInfoList)) {
        outlog("[错误] 保存翻译后的文件失败");
        return false;
    }
    
    // 计算总耗时
    int totalTime = startTime.msecsTo(QTime::currentTime());
    QString totalTimeStr;
    if (totalTime < 1000) {
        totalTimeStr = QString("%1毫秒").arg(totalTime);
    } else if (totalTime < 60000) {
        totalTimeStr = QString("%1秒").arg(totalTime / 1000);
    } else if (totalTime < 3600000) {
        int minutes = totalTime / 60000;
        int seconds = (totalTime % 60000) / 1000;
        totalTimeStr = QString("%1分%2秒").arg(minutes).arg(seconds);
    } else {
        int hours = totalTime / 3600000;
        int minutes = (totalTime % 3600000) / 60000;
        totalTimeStr = QString("%1小时%2分").arg(hours).arg(minutes);
    }
    
    outlog("\n=== 翻译任务完成 ===");
    outlog("输出文件: " + outputFilePath);
    outlog(QString("成功翻译 %1 条字幕").arg(srtInfoList.size()));
    outlog(QString("总耗时: %1").arg(totalTimeStr));
    return true;
}

QString SrtTranslator::buildPrompt(const QString& content, const PromptInfo& promptInfo)
{
    QString fullPrompt;
    
    // 添加聊天前缀
    if (!promptInfo.chatPrefix.isEmpty()) {
        fullPrompt += promptInfo.chatPrefix + "\n\n";
    }
    
    // 添加需要翻译的内容
    fullPrompt += content + "\n\n";
    
    // 添加聊天后缀
    if (!promptInfo.chatSuffix.isEmpty()) {
        fullPrompt += promptInfo.chatSuffix;
    }
    
    return fullPrompt;
}

void SrtTranslator::outlog(const QString& log)
{
    QString formattedLog = QTime::currentTime().toString("[hh:mm:ss] ") + log;
    qDebug() << formattedLog;
    if (m_logOutput) {
        m_logOutput->appendPlainText(formattedLog);
    }
    QCoreApplication::processEvents();
} 