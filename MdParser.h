#ifndef MDPARSER_H
#define MDPARSER_H

#include <QList>
#include <QString>
#include <QStringList>
#include <QVector>

// MD信息结构体
struct MdInfo
{
    QString content;          // 原内容
    QString translateContent; // 翻译内容
};

class MdParser
{
public:
    MdParser();
    ~MdParser();

    // 解析MD文件
    bool parse(const QString &filePath);

    // 保存MD文件
    bool save(const QString &filePath, const QVector<MdInfo> &mdInfoList);

    // 获取解析后的文本列表
    QVector<MdInfo> getMdInfoList() const
    {
        return m_mdInfoList;
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
    QVector<MdInfo> m_mdInfoList;

    // 段落长度限制
    int m_maxLen = 3072; // 最大段落长度
    int m_minLen = 1024; // 最小段落长度
};

#endif // MDPARSER_H 