#ifndef RSTPARSER_H
#define RSTPARSER_H

#include <QList>
#include <QString>
#include <QStringList>
#include <QVector>

// RST信息结构体
struct RstInfo
{
    QString content;          // 原内容
    QString translateContent; // 翻译内容
};

class RstParser
{
public:
    RstParser();
    ~RstParser();

    // 解析RST文件
    bool parse(const QString &filePath);

    // 保存RST文件
    bool save(const QString &filePath, const QVector<RstInfo> &rstInfoList);

    // 获取解析后的文本列表
    QVector<RstInfo> getRstInfoList() const
    {
        return m_rstInfoList;
    }

    // 设置最大段落长度
    void setMaxLength(int length)
    {
        m_maxLen = length;
    }

    // 设置最小段落长度
    void setMinLength(int length)
    {
        m_minLen = length;
    }

    // 获取最大段落长度
    int getMaxLength() const
    {
        return m_maxLen;
    }

    // 获取最小段落长度
    int getMinLength() const
    {
        return m_minLen;
    }

private:
    // 将文本分割成段落
    QList<QString> splitIntoParagraphs(const QString &content);

    // 将段落处理成合适的长度
    QList<QString> processParagraphs(const QList<QString> &paragraphs);

    // 存储解析后的文本信息
    QVector<RstInfo> m_rstInfoList;

    // 段落长度限制
    int m_maxLen = 3072; // 最大段落长度
    int m_minLen = 1024; // 最小段落长度
};

#endif // RSTPARSER_H 