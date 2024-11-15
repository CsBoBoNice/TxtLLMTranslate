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

    qDebug() << "原始内容:" << content;

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

    // 使用正则表达式匹配Markdown标题和代码块
    QRegularExpression titleRegex(R"(^#{1,6}\s+.*$)", QRegularExpression::MultilineOption);
    QRegularExpression codeBlockRegex(R"(^```.*$)");

    // 按行分割内容
    QStringList lines = content.split('\n');

    QString currentParagraph;
    bool inCodeBlock = false;  // 标记是否在代码块内

    for (int i = 0; i < lines.size(); ++i)
    {
        QString line = lines[i];
        
        // 检查是否是代码块标记
        if (codeBlockRegex.match(line.trimmed()).hasMatch())
        {
            // 如果已经在代码块内，说明这是结束标记
            if (inCodeBlock)
            {
                currentParagraph += line + "\n";
                paragraphs.append(currentParagraph);
                currentParagraph.clear();
                inCodeBlock = false;
                continue;
            }
            // 否则这是开始标记
            else
            {
                // 保存之前的非代码块段落（如果存在）
                if (!currentParagraph.isEmpty())
                {
                    paragraphs.append(currentParagraph);
                    currentParagraph.clear();
                }
                currentParagraph = line + "\n";
                inCodeBlock = true;
                continue;
            }
        }

        // 代码块内的处理逻辑
        if (inCodeBlock)
        {
            // 检查添加当前行是否会超过最大长度
            if (currentParagraph.length() + line.length() > m_maxLen)
            {
                // 在当前段落末尾添加代码块结束标记
                currentParagraph += "```\n\n";
                paragraphs.append(currentParagraph);
                // 新段落以代码块开始标记开头
                currentParagraph = "```\n" + line + "\n";
            }
            else
            {
                currentParagraph += line + "\n";
            }
            continue;
        }

        // 非代码块内的处理逻辑
        // 检查是否是标题行
        if (titleRegex.match(line.trimmed()).hasMatch())
        {
            if (!currentParagraph.isEmpty())
            {
                paragraphs.append(currentParagraph);
                currentParagraph.clear();
            }
            currentParagraph = line + "\n";
            continue;
        }

        // 处理普通行
        if (currentParagraph.length() + line.length() > m_maxLen)
        {
            if (!currentParagraph.isEmpty())
            {
                paragraphs.append(currentParagraph);
                currentParagraph.clear();
            }
        }

        // 添加当前行
        if (currentParagraph.isEmpty())
        {
            currentParagraph = line + "\n";
        }
        else
        {
            currentParagraph += line + "\n";
        }

        // 当前行以句号结尾（仅在非代码块内判断）
        if (line.trimmed().endsWith("。") || line.trimmed().endsWith("."))
        {
            paragraphs.append(currentParagraph);
            currentParagraph.clear();
        }
    }

    // 处理最后一个段落
    if (!currentParagraph.isEmpty())
    {
        // 如果最后在代码块内，确保添加结束标记
        if (inCodeBlock && !currentParagraph.trimmed().endsWith("```"))
        {
            currentParagraph += "```\n";
        }
        paragraphs.append(currentParagraph);
    }

    // 输出段落信息用于调试
    qDebug() << "分段完成，总段落数:" << paragraphs.size();
    qDebug() << "---段落详细信息---";
    for (int i = 0; i < paragraphs.size(); ++i)
    {
        qDebug() << "段落" << i + 1 << ":";
        qDebug() << "内容:" << paragraphs[i];
        qDebug() << "长度:" << paragraphs[i].length();
        qDebug() << "是否为空行:" << paragraphs[i].trimmed().isEmpty();
        qDebug() << "----------------";
    }

    return paragraphs;
}

QList<QString> MdParser::processParagraphs(const QList<QString> &paragraphs)
{
    QList<QString> processedParagraphs;
    QString currentParagraph;

    for (const QString &paragraph : paragraphs)
    {

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
                currentParagraph += paragraph;
            }
        }
        // 如果当前段落已经达到或超过最小长度，
        else if (currentParagraph.length() >= m_minLen)
        {
            processedParagraphs.append(currentParagraph);
            currentParagraph = paragraph;
        }
        // 如果当前段落小于最小长度，强制合并
        else
        {
            currentParagraph += paragraph;
        }
    }

    // 处理最后一个段落 - 直接添加，不进行合并
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
    }

    file.close();
    qDebug() << "成功保存MD文件:" << filePath;

    // // 读取保存的文件，验证内容

    // QFile file_read(filePath);
    // if (!file_read.open(QIODevice::ReadOnly | QIODevice::Text))
    // {
    //     qDebug() << "无法打开MD文件:" << filePath;
    //     return false;
    // }

    // QTextStream in(&file_read);
    // in.setEncoding(QStringConverter::Utf8);

    // // 读取整个文件内容
    // QString content = in.readAll();
    // file_read.close();

    // qDebug() << "保存内容:" << content;

    return true;
}