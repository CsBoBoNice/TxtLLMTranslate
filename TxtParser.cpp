#include "TxtParser.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

TxtParser::TxtParser()
{
    qDebug() << "初始化TXT解析器";
    m_maxLen = 2000;  // 设置默认最大长度
    m_minLen = 100;   // 设置默认最小长度
}

TxtParser::~TxtParser()
{
}

bool TxtParser::parse(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开TXT文件:" << filePath;
        return false;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    
    // 读取整个文件内容
    QString content = in.readAll();
    file.close();

    // 将文本分割成初始段落
    QList<QString> initialParagraphs = splitIntoParagraphs(content);
    qDebug() << "初始分段完成，共" << initialParagraphs.size() << "个段落";

    // 处理段落长度
    QList<QString> processedParagraphs = processParagraphs(initialParagraphs);
    qDebug() << "段落处理完成，最终共" << processedParagraphs.size() << "个段落";

    // 将处理后的段落保存到m_txtInfoList
    m_txtInfoList.clear();
    for (const QString& paragraph : processedParagraphs) {
        TxtInfo info;
        info.content = paragraph;
        m_txtInfoList.append(info);
    }

    qDebug() << "成功解析TXT文件，共" << m_txtInfoList.size() << "个段落";
    return true;
}

QList<QString> TxtParser::splitIntoParagraphs(const QString& content)
{
    QList<QString> paragraphs;
    QString currentParagraph;
    
    QStringList lines = content.split(QRegularExpression("[\r\n]"), Qt::SkipEmptyParts);
    
    for (const QString& line : lines) {
        QString trimmedLine = line.trimmed();
        if (trimmedLine.isEmpty()) {
            // 空行表示段落结束
            if (!currentParagraph.isEmpty()) {
                paragraphs.append(currentParagraph);
                currentParagraph.clear();
            }
        } else {
            // 处理句号分割
            QStringList sentences = trimmedLine.split('.', Qt::SkipEmptyParts);
            for (const QString& sentence : sentences) {
                QString trimmedSentence = sentence.trimmed();
                if (!trimmedSentence.isEmpty()) {
                    if (currentParagraph.length() + trimmedSentence.length() > m_maxLen) {
                        if (!currentParagraph.isEmpty()) {
                            paragraphs.append(currentParagraph);
                            currentParagraph.clear();
                        }
                    }
                    if (!currentParagraph.isEmpty()) {
                        currentParagraph += " ";
                    }
                    currentParagraph += trimmedSentence + ".";
                }
            }
        }
    }
    
    // 处理最后一个段落
    if (!currentParagraph.isEmpty()) {
        paragraphs.append(currentParagraph);
    }
    
    return paragraphs;
}

QList<QString> TxtParser::processParagraphs(const QList<QString>& paragraphs)
{
    QList<QString> processedParagraphs;
    QString currentParagraph;
    
    for (const QString& paragraph : paragraphs) {
        if (currentParagraph.length() + paragraph.length() <= m_maxLen) {
            // 可以合并段落
            if (!currentParagraph.isEmpty()) {
                currentParagraph += " ";
            }
            currentParagraph += paragraph;
        } else {
            // 当前段落已达到合适长度，保存并开始新段落
            if (!currentParagraph.isEmpty()) {
                processedParagraphs.append(currentParagraph);
            }
            currentParagraph = paragraph;
        }
        
        // 如果当前段落已经足够长，保存并重置
        if (currentParagraph.length() >= m_minLen) {
            processedParagraphs.append(currentParagraph);
            currentParagraph.clear();
        }
    }
    
    // 处理最后一个段落
    if (!currentParagraph.isEmpty()) {
        processedParagraphs.append(currentParagraph);
    }
    
    return processedParagraphs;
}

bool TxtParser::save(const QString& filePath, const QVector<TxtInfo>& txtInfoList)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "无法创建TXT文件:" << filePath;
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    for (const TxtInfo& info : txtInfoList) {
        // 写入翻译内容，如果没有则写入原文
        if (!info.translateContent.isEmpty()) {
            out << info.translateContent;
        } else {
            out << info.content;
        }
        out << "\n\n";  // 段落之间添加空行
    }

    file.close();
    qDebug() << "成功保存TXT文件:" << filePath;
    return true;
} 