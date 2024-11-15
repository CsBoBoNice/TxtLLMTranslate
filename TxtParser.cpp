#include "TxtParser.h"
#include <QDebug>
#include <QFile>
#include <QRegularExpression>
#include <QStringConverter>
#include <QTextStream>

TxtParser::TxtParser()
{
    qDebug() << "初始化TXT解析器";
    m_maxLen = 3072; // 设置默认最大长度
    m_minLen = 1024;  // 设置默认最小长度
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
    
    // 按行分割内容
    QStringList lines = content.split('\n');
    
    QString currentParagraph;
    
    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines[i];
        
        // 添加当前行（保留原始格式，包括缩进）
        if (currentParagraph.isEmpty()) {
            currentParagraph = line + "\n";
        } else {
            currentParagraph += line + "\n";
        }
        
        // 检查是否需要结束当前段落
        bool shouldEndParagraph = false;
        
        // 情况1: 当前行是空行
        if (line.trimmed().isEmpty()) {
            shouldEndParagraph = true;
        }
        // 情况2: 当前行以句号结尾
        else if (line.trimmed().endsWith("。") || line.trimmed().endsWith(".")) {
            shouldEndParagraph = true;
        }
        // 情况3: 当前段落长度超过最大长度
        else if (currentParagraph.length() > m_maxLen) {
            shouldEndParagraph = true;
        }
        
        // 如果需要结束段落，保存当前段落并清空
        if (shouldEndParagraph && !currentParagraph.isEmpty()) {
            paragraphs.append(currentParagraph);
            currentParagraph.clear();
        }
    }
    
    // 处理最后一个段落
    if (!currentParagraph.isEmpty()) {
        paragraphs.append(currentParagraph);
    }
    
    // 输出段落信息用于调试
    qDebug() << "分段完成，总段落数:" << paragraphs.size();
    for (int i = 0; i < paragraphs.size(); ++i) {
        qDebug() << QString("段落 %1 (长度: %2):").arg(i + 1).arg(paragraphs[i].length());
        qDebug() << "内容预览:" << paragraphs[i];
        qDebug() << "是否以句号结尾:" << (paragraphs[i].trimmed().endsWith("。") || paragraphs[i].trimmed().endsWith("."));
        qDebug() << "是否包含空行:" << paragraphs[i].contains(QRegularExpression("^\\s*$", QRegularExpression::MultilineOption));
        qDebug() << "----------------";
    }
    
    return paragraphs;
}

QList<QString> TxtParser::processParagraphs(const QList<QString> &paragraphs)
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