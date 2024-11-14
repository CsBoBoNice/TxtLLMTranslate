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
    QStringList lines = content.split('\n');
    QString currentParagraph;
    bool isInCodeBlock = false;

    // 遍历每一行
    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines[i];
        
        // 如果遇到新的标题行，且不在代码块内
        if (line.startsWith('#') && !isInCodeBlock) {
            // 保存之前的段落（如果存在）
            if (!currentParagraph.isEmpty()) {
                paragraphs.append(currentParagraph);
                currentParagraph.clear();
            }
            // 开始新的段落，以标题行开始
            currentParagraph = line;
            continue;
        }

        // 处理代码块标记
        if (line.startsWith("```")) {
            isInCodeBlock = !isInCodeBlock;
            currentParagraph += (currentParagraph.isEmpty() ? "" : "\n") + line;
            continue;
        }

        // 在代码块内的内容直接添加到当前段落
        if (isInCodeBlock) {
            currentParagraph += "\n" + line;
            continue;
        }

        // 处理空行
        if (line.trimmed().isEmpty()) {
            currentParagraph += "\n" + line;
            continue;
        }

        // 处理普通文本行
        if (currentParagraph.isEmpty()) {
            currentParagraph = line;
        } else {
            currentParagraph += "\n" + line;
        }
    }

    // 处理最后一个段落
    if (!currentParagraph.isEmpty()) {
        paragraphs.append(currentParagraph);
    }

    // 输出调试信息
    qDebug() << "分段完成，总段落数:" << paragraphs.size();
    qDebug() << "---段落详细信息---";
    for (int i = 0; i < paragraphs.size(); ++i) {
        qDebug() << "段落" << i + 1 << ":";
        qDebug() << "内容:" << "\"" + paragraphs[i] + "\"";
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
                currentParagraph = "\n" + paragraph;
            }
            else
            {
                currentParagraph += "\n" + paragraph;
            }
        }
        // 如果当前段落已经达到或超过最小长度，
        else if (currentParagraph.length() >= m_minLen)
        {
            processedParagraphs.append(currentParagraph);
            currentParagraph = "\n" + paragraph;
        }
        // 如果当前段落小于最小长度，强制合并
        else
        {
            currentParagraph += "\n" + paragraph;
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