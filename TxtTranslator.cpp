#include "TxtTranslator.h"
#include <QCoreApplication>
#include <QDebug>
#include <QStringList>
#include <QTime>

TxtTranslator::TxtTranslator(int maxLen, int minLen)
    : m_openaiManager(OpenaiManager::getInstance()), m_translatePrompt(TxtPrompt::getInstance()),
      m_maxLen(maxLen), m_minLen(minLen)
{
    qDebug() << "初始化TXT文件翻译";
    m_translatePrompt->load(m_translatePrompt->getPromptPath());
    m_txtParser.setMaxLength(m_maxLen);
    m_txtParser.setMinLength(m_minLen);
}

TxtTranslator::~TxtTranslator() {}

bool TxtTranslator::translate(const QString &inputFilePath, const QString &outputFilePath,
                              const QString &url, const QString &apiKey, const QString &model,
                              bool keepHistory)
{
    QTime startTime = QTime::currentTime();
    outlog("=== 开始翻译任务 ===");
    outlog("文本文件: " + inputFilePath);
    outlog("翻译模型: " + model);
    outlog("");

    // 解析TXT文件
    if (!m_txtParser.parse(inputFilePath))
    {
        outlog("[错误] 解析TXT文件失败");
        return false;
    }

    // 获取文本列表
    QVector<TxtInfo> txtInfoList = m_txtParser.getTxtInfoList();
    outlog(QString("共找到 %1 个段落").arg(txtInfoList.size()));
    outlog("-------------------");

    // 获取翻译提示信息
    PromptInfo promptInfo = m_translatePrompt->getPrompt();

    // 用于计算平均处理时间
    QVector<int> processingTimes;

    // 遍历文本列表进行翻译
    for (int i = 0; i < txtInfoList.size(); ++i)
    {
        QTime itemStartTime = QTime::currentTime();

        // 处理用户提示列表
        QVector<QString> processedUserPrompts;
        QVector<QString> processedAssistantPrompts;

        // 先加入历史记录
        for (const TranslateHistory &history : m_history)
        {
            processedUserPrompts.append(history.original);
            processedAssistantPrompts.append(history.translated);
        }

        // 然后增加参考示例
        for (const QString &userPrompt : promptInfo.userPromptList)
        {
            QString processedPrompt = buildPrompt(userPrompt, promptInfo);
            processedUserPrompts.append(processedPrompt);
        }

        for (const QString &assistantPrompt : promptInfo.assistantPromptList)
        {
            processedAssistantPrompts.append(assistantPrompt);
        }

        outlog(QString("\n[进度 %1/%2]").arg(i + 1).arg(txtInfoList.size()));
        outlog("原文: " + txtInfoList[i].content);

        // 构建当前段落的翻译提示
        QString currentPrompt = buildPrompt(txtInfoList[i].content, promptInfo);

        // 创建新的用户提示列表，包含所有处理过的示例和当前段落
        QVector<QString> finalUserPrompts = processedUserPrompts;
        finalUserPrompts.append(currentPrompt);

        // 调用OpenAI API进行翻译
        QString translation = m_openaiManager->sendMessage(
            url, apiKey, model, promptInfo.prompt, finalUserPrompts, processedAssistantPrompts);

        if (translation.isEmpty())
        {
            outlog("[错误] 翻译失败，跳过当前段落");
            outlog("-------------------");
            continue;
        }

        QString validTranslation = translation;

        outlog("译文: " + validTranslation);

        // 保存翻译结果
        if (!validTranslation.isEmpty())
        {
            txtInfoList[i].translateContent = validTranslation;
            if (keepHistory)
            {
                TranslateHistory history;
                history.original   = currentPrompt;
                history.translated = validTranslation;
                m_history.append(history);

                // 保持历史记录不超过限制
                const int maxHistory = 5;
                while (m_history.size() > maxHistory)
                {
                    m_history.removeFirst();
                }
            }
        }
        else
        {
            outlog("[警告] 未找到有效的翻译内容，使用原文");
            txtInfoList[i].translateContent = txtInfoList[i].content;
        }

        // 计算本段落的处理时间
        int processTime = itemStartTime.msecsTo(QTime::currentTime());
        processingTimes.append(processTime);

        // 计算平均处理时间和预计剩余时间
        if (processingTimes.size() > 0)
        {
            // 计算平均处理时间（毫秒）
            qint64 avgTime = 0;
            for (int time : processingTimes)
            {
                avgTime += time;
            }
            avgTime /= processingTimes.size();

            // 计算预计剩余时间
            int remainingItems            = txtInfoList.size() - (i + 1);
            qint64 estimatedRemainingTime = avgTime * remainingItems;

            // 转换为更友好的时间格式
            QString timeStr;
            if (estimatedRemainingTime < 1000)
            {
                timeStr = QString("%1毫秒").arg(estimatedRemainingTime);
            }
            else if (estimatedRemainingTime < 60000)
            {
                timeStr = QString("%1秒").arg(estimatedRemainingTime / 1000);
            }
            else if (estimatedRemainingTime < 3600000)
            {
                int minutes = estimatedRemainingTime / 60000;
                int seconds = (estimatedRemainingTime % 60000) / 1000;
                timeStr     = QString("%1分%2秒").arg(minutes).arg(seconds);
            }
            else
            {
                int hours   = estimatedRemainingTime / 3600000;
                int minutes = (estimatedRemainingTime % 3600000) / 60000;
                timeStr     = QString("%1小时%2分").arg(hours).arg(minutes);
            }

            outlog(QString("平均处理时间: %1ms").arg(avgTime));
            outlog(QString("预计剩余时间: %1").arg(timeStr));
        }

        outlog("-------------------");
    }

    if (!m_txtParser.save(outputFilePath, txtInfoList))
    {
        outlog("[错误] 保存翻译后的文件失败");
        return false;
    }

    // 计算总耗时
    int totalTime = startTime.msecsTo(QTime::currentTime());
    QString totalTimeStr;
    if (totalTime < 1000)
    {
        totalTimeStr = QString("%1毫秒").arg(totalTime);
    }
    else if (totalTime < 60000)
    {
        totalTimeStr = QString("%1秒").arg(totalTime / 1000);
    }
    else if (totalTime < 3600000)
    {
        int minutes  = totalTime / 60000;
        int seconds  = (totalTime % 60000) / 1000;
        totalTimeStr = QString("%1分%2秒").arg(minutes).arg(seconds);
    }
    else
    {
        int hours    = totalTime / 3600000;
        int minutes  = (totalTime % 3600000) / 60000;
        totalTimeStr = QString("%1小时%2分").arg(hours).arg(minutes);
    }

    outlog("\n=== 翻译任务完成 ===");
    outlog("输出文件: " + outputFilePath);
    outlog(QString("成功翻译 %1 个段落").arg(txtInfoList.size()));
    outlog(QString("总耗时: %1").arg(totalTimeStr));
    return true;
}

QString TxtTranslator::buildPrompt(const QString &content, const PromptInfo &promptInfo)
{
    QString fullPrompt;

    // 添加聊天前缀
    if (!promptInfo.chatPrefix.isEmpty())
    {
        fullPrompt += promptInfo.chatPrefix + "\n\n";
    }

    // 添加需要翻译的内容
    fullPrompt += content + "\n\n";

    // 添加聊天后缀
    if (!promptInfo.chatSuffix.isEmpty())
    {
        fullPrompt += promptInfo.chatSuffix;
    }

    return fullPrompt;
}

void TxtTranslator::outlog(const QString &log)
{
    QString formattedLog = QTime::currentTime().toString("[hh:mm:ss] ") + log;
    qDebug() << formattedLog;
    if (m_logOutput)
    {
        m_logOutput->appendPlainText(formattedLog);
    }
    QCoreApplication::processEvents();
}