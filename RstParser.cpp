#include "RstParser.h"
#include <QDebug>
#include <QFile>
#include <QRegularExpression>
#include <QStringConverter>
#include <QTextStream>

RstParser::RstParser()
{
    qDebug() << "初始化RST解析器";
    m_maxLen = 3072; // 设置默认最大长度
    m_minLen = 1024; // 设置默认最小长度
}

RstParser::~RstParser() {}

bool RstParser::parse(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "无法打开RST文件:" << filePath;
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

    // 将处理后的段落保存到m_rstInfoList
    m_rstInfoList.clear();
    for (const QString &paragraph : processedParagraphs)
    {
        RstInfo info;
        info.content = paragraph;
        m_rstInfoList.append(info);
    }

    qDebug() << "成功解析RST文件，共" << m_rstInfoList.size() << "个段落";
    return true;
}

QList<QString> RstParser::splitIntoParagraphs(const QString &content)
{
    QList<QString> paragraphs;

    // 使用正则表达式匹配RST标题行
    QRegularExpression titlePattern("^[=\\-`:\\'\\\"~\\^_\\*\\+#<>]+$");

    // 按行分割内容
    QStringList lines = content.split('\n');

    QString currentParagraph;
    QString lastLine;
    QString nowLine;

    for (int i = 0; i < lines.size(); ++i)
    {
        nowLine = lines[i];

        if (lastLine.isEmpty())
        {
            lastLine = nowLine + "\n";
            continue;
        }

        // 检查是否是标题行
        if (titlePattern.match(nowLine.trimmed()).hasMatch() && !lastLine.trimmed().isEmpty() &&
            nowLine.trimmed().length() >= lastLine.trimmed().length())
        {
            // 当前行是标题的下划线，lastLine是标题文本
            if (!currentParagraph.isEmpty())
            {
                paragraphs.append(currentParagraph);
                currentParagraph.clear();
            }

            // 将标题和下划线作为一个段落
            currentParagraph = lastLine;
            lastLine         = nowLine + "\n";
            continue;
        }

        // 检查是否会超过最大长度
        if (currentParagraph.length() + lastLine.length() > m_maxLen)
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
            currentParagraph = lastLine;
        }
        else
        {
            currentParagraph += lastLine;
        }

        // 以句号结尾
        if (lastLine.trimmed().endsWith("."))
        {
            if (!currentParagraph.isEmpty())
            {
                paragraphs.append(currentParagraph);
                currentParagraph.clear();
            }
        }

        lastLine = nowLine + "\n";
    }

    // 处理最后一个段落
    if (currentParagraph.isEmpty())
    {
        currentParagraph = lastLine + nowLine + "\n";
    }
    else
    {
        currentParagraph += lastLine + nowLine + "\n";
    }

    paragraphs.append(currentParagraph);

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

QList<QString> RstParser::processParagraphs(const QList<QString> &paragraphs)
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

bool RstParser::save(const QString &filePath, const QVector<RstInfo> &rstInfoList)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "无法创建RST文件:" << filePath;
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    for (const RstInfo &info : rstInfoList)
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
    qDebug() << "成功保存RST文件:" << filePath;
    return true;
}