#include "MdParser.h"
#include <QDebug>
#include <QFile>
#include <QRegularExpression>
#include <QStringConverter>
#include <QTextStream>

MdParser::MdParser()
{
    qDebug() << "初始化MD解析器";
    m_maxLen = 3072; // 设置默认最大长度
    m_minLen = 1024; // 设置默认最小长度
}

MdParser::~MdParser() {}

bool MdParser::parse(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "无法打开MD文件:" << filePath;
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

    // 将处理后的段落保存到m_mdInfoList
    m_mdInfoList.clear();
    for (const QString &paragraph : processedParagraphs)
    {
        MdInfo info;
        info.content = paragraph;
        m_mdInfoList.append(info);
    }

    qDebug() << "成功解析MD文件，共" << m_mdInfoList.size() << "个段落";
    return true;
}

QList<QString> MdParser::splitIntoParagraphs(const QString &content)
{
    QList<QString> paragraphs;
    QString currentParagraph;

    // 按行分割，保留所有换行符
    QStringList lines = content.split(QRegularExpression("\r\n|\n"), Qt::KeepEmptyParts);

    // 用于检测Markdown标题的正则表达式
    QRegularExpression headerRegex("^#{1,6}\\s");

    for (int i = 0; i < lines.size(); ++i)
    {
        const QString &line = lines[i];
        bool isEmptyLine = line.trimmed().isEmpty();
        bool isHeader = headerRegex.match(line).hasMatch();

        // 如果遇到标题或空行，保存当前段落
        if (isHeader || isEmptyLine)
        {
            if (!currentParagraph.isEmpty())
            {
                paragraphs.append(currentParagraph);
                currentParagraph.clear();
            }
            // 如果是标题，单独作为一个段落
            if (isHeader)
            {
                paragraphs.append(line);
            }
            else if (isEmptyLine)
            {
                // 保存空行本身
                paragraphs.append(line);
            }
        }
        else
        {
            // 处理普通行
            if (currentParagraph.isEmpty())
            {
                currentParagraph = line;
            }
            else
            {
                // 检查是否需要分段（行末尾为句号或达到最大长度）
                QString trimmedLine = line.trimmed();
                bool endsWithPeriod = !trimmedLine.isEmpty() && 
                    (trimmedLine.endsWith(QLatin1Char('.')) || 
                     trimmedLine.endsWith(QString::fromUtf8("。")));

                if (endsWithPeriod || currentParagraph.length() + line.length() + 1 > m_maxLen)
                {
                    paragraphs.append(currentParagraph);
                    currentParagraph = line;
                }
                else
                {
                    // 在同一段落内添加新行，保持原始格式
                    currentParagraph += "\n" + line;
                }
            }
        }
    }

    // 处理最后一个段落
    if (!currentParagraph.isEmpty())
    {
        paragraphs.append(currentParagraph);
    }

    return paragraphs;
}

QList<QString> MdParser::processParagraphs(const QList<QString> &paragraphs)
{
    QList<QString> processedParagraphs;
    QString currentParagraph;

    // 用于检测Markdown标题的正则表达式
    QRegularExpression headerRegex("^#{1,6}\\s");

    for (const QString &paragraph : paragraphs)
    {
        // 如果是标题或空行，直接添加不进行合并
        if (headerRegex.match(paragraph).hasMatch() || paragraph.trimmed().isEmpty())
        {
            if (!currentParagraph.isEmpty())
            {
                processedParagraphs.append(currentParagraph);
                currentParagraph.clear();
            }
            processedParagraphs.append(paragraph);
            continue;
        }

        // 如果合并后的长度小于最大长度，尝试合并
        if (currentParagraph.length() + paragraph.length() <= m_maxLen)
        {
            if (currentParagraph.length() >= m_minLen)
            {
                processedParagraphs.append(currentParagraph);
                currentParagraph = paragraph;
            }
            else
            {
                if (currentParagraph.isEmpty())
                {
                    currentParagraph = paragraph;
                }
                else
                {
                    currentParagraph += "\n" + paragraph;
                }
            }
        }
        // 如果当前段落已经达到或超过最小长度
        else if (currentParagraph.length() >= m_minLen)
        {
            processedParagraphs.append(currentParagraph);
            currentParagraph = paragraph;
        }
        // 如果当前段落小于最小长度，强制合并
        else
        {
            if (currentParagraph.isEmpty())
            {
                currentParagraph = paragraph;
            }
            else
            {
                currentParagraph += "\n" + paragraph;
            }
        }
    }

    // 处理最后一个段落
    if (!currentParagraph.isEmpty())
    {
        processedParagraphs.append(currentParagraph);
    }

    return processedParagraphs;
}

bool MdParser::save(const QString &filePath, const QVector<MdInfo> &mdInfoList)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "无法创建MD文件:" << filePath;
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    for (const MdInfo &info : mdInfoList)
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
        
        // 如果不是最后一个段落，添加换行符
        if (&info != &mdInfoList.last())
        {
            out << "\n";
            // 如果当前段落不是空行，额外添加一个换行符
            if (!info.content.trimmed().isEmpty())
            {
                out << "\n";
            }
        }
    }

    file.close();
    qDebug() << "成功保存MD文件:" << filePath;
    return true;
} 