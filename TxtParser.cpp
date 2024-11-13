#include "TxtParser.h"
#include <QDebug>
#include <QFile>
#include <QRegularExpression>
#include <QStringConverter>
#include <QTextStream>

TxtParser::TxtParser()
{
    qDebug() << "初始化TXT解析器";
    m_maxLen = 2000; // 设置默认最大长度
    m_minLen = 100;  // 设置默认最小长度
}

TxtParser::~TxtParser() {}

bool TxtParser::parse(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
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
    for (const QString &paragraph : processedParagraphs)
    {
        TxtInfo info;
        info.content = paragraph;
        m_txtInfoList.append(info);
    }

    qDebug() << "成功解析TXT文件，共" << m_txtInfoList.size() << "个段落";
    return true;
}

QList<QString> TxtParser::splitIntoParagraphs(const QString &content)
{
    QList<QString> paragraphs;
    QString currentParagraph;
    int emptyLineCount = 0;  // 用于追踪连续空行数量

    // 按行分割，保留所有换行符
    QStringList lines = content.split(QRegularExpression("(\r\n|\n)"), Qt::KeepEmptyParts);

    for (int i = 0; i < lines.size(); ++i)
    {
        const QString &line = lines[i];
        bool isEmptyLine = line.trimmed().isEmpty();

        if (isEmptyLine)
        {
            emptyLineCount++;
            // 如果当前段落不为空且遇到空行，保存当前段落
            if (!currentParagraph.isEmpty())
            {
                // 添加段落及其后的空行
                currentParagraph += QString("\n").repeated(emptyLineCount);
                paragraphs.append(currentParagraph);
                currentParagraph.clear();
            }
            continue;
        }

        // 重置空行计数
        emptyLineCount = 0;

        // 处理非空行
        if (currentParagraph.isEmpty())
        {
            currentParagraph = line;
        }
        else
        {
            // 检查是否有句号
            QStringList sentences = line.split(QRegularExpression("[.。]"), Qt::KeepEmptyParts);
            for (const QString &sentence : sentences)
            {
                if (sentence.trimmed().isEmpty()) continue;

                // 检查添加新内容是否会超过最大长度
                if (currentParagraph.length() + sentence.length() + 1 > m_maxLen)
                {
                    // 如果当前段落不为空，保存它
                    if (!currentParagraph.isEmpty())
                    {
                        paragraphs.append(currentParagraph);
                        currentParagraph.clear();
                    }
                    currentParagraph = sentence;
                }
                else
                {
                    // 在句子之间添加原始的句号
                    if (!currentParagraph.isEmpty())
                    {
                        currentParagraph += ".";
                    }
                    currentParagraph += sentence;
                }
            }

            // 如果原行以句号结尾，确保保留
            if (line.endsWith(".") || line.endsWith("。"))
            {
                currentParagraph += ".";
            }
        }

        // 检查是否达到最大长度
        if (currentParagraph.length() >= m_maxLen)
        {
            paragraphs.append(currentParagraph);
            currentParagraph.clear();
        }
    }

    // 处理最后一个段落
    if (!currentParagraph.isEmpty())
    {
        // 如果文件以空行结尾，添加空行
        if (emptyLineCount > 0)
        {
            currentParagraph += QString("\n").repeated(emptyLineCount);
        }
        paragraphs.append(currentParagraph);
    }

    return paragraphs;
}

QList<QString> TxtParser::processParagraphs(const QList<QString> &paragraphs)
{
    QList<QString> processedParagraphs;
    QString currentParagraph;

    for (const QString &paragraph : paragraphs)
    {
        // 跳过空段落
        if (paragraph.trimmed().isEmpty())
        {
            continue;
        }

        // 如果当前段落为空，直接使用新段落
        if (currentParagraph.isEmpty())
        {
            currentParagraph = paragraph;
        }
        // 如果合并后的长度小于最大长度，尝试合并
        else if (currentParagraph.length() + paragraph.length() <= m_maxLen)
        {
            currentParagraph += "\n" + paragraph;
        }
        // 如果当前段落已经达到或超过最小长度，保存并开始新段落
        else if (currentParagraph.length() >= m_minLen)
        {
            processedParagraphs.append(currentParagraph);
            currentParagraph = paragraph;
        }
        // 如果当前段落小于最小长度，强制合并
        else
        {
            currentParagraph += "\n" + paragraph;
        }

        // 如果当前段落超过最大长度，需要进行分割
        while (currentParagraph.length() > m_maxLen)
        {
            // 找到最后一个完整句子的位置
            int lastDot = currentParagraph.lastIndexOf('.', m_maxLen);
            if (lastDot == -1 || lastDot < m_minLen)
            {
                // 如果找不到合适的句号，就在最大长度处强制截断
                lastDot = m_maxLen;
            }

            // 保存当前部分
            QString part = currentParagraph.left(lastDot + 1);
            processedParagraphs.append(part);

            // 更新剩余部分
            currentParagraph = currentParagraph.mid(lastDot + 1).trimmed();
        }
    }

    // 处理最后一个段落 - 直接添加，不进行合并
    if (!currentParagraph.isEmpty())
    {
        processedParagraphs.append(currentParagraph);
    }

    return processedParagraphs;
}

bool TxtParser::save(const QString &filePath, const QVector<TxtInfo> &txtInfoList)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "无法创建TXT文件:" << filePath;
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    for (const TxtInfo &info : txtInfoList)
    {
        // 写入翻译内容，如果没有则写入原文
        if (!info.translateContent.isEmpty())
        {
            out << info.translateContent;
        }
        else
        {
            out << info.content;
        }
        out << "\n\n"; // 段落之间添加空行
    }

    file.close();
    qDebug() << "成功保存TXT文件:" << filePath;
    return true;
}