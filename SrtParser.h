#ifndef SRTPARSER_H
#define SRTPARSER_H

#include <QString>
#include <QVector>

// SRT信息结构体
struct SrtInfo
{
    QString time;             // 字幕时间
    QString content;          // 字幕原内容
    QString translateContent; // 字幕翻译内容
};

class SrtParser
{
public:
    SrtParser();
    ~SrtParser();

    // 解析SRT文件
    bool parse(const QString &filePath);

    // 保存SRT文件
    bool save(const QString &filePath, const QVector<SrtInfo> &srtInfoList);

    // 获取解析后的字幕列表
    QVector<SrtInfo> getSrtInfoList() const
    {
        return m_srtInfoList;
    }

private:
    // 解析单个时间字符串
    bool parseTimeString(const QString &timeStr, QString &time);

    // 存储解析后的字幕信息
    QVector<SrtInfo> m_srtInfoList;
};

#endif // SRTPARSER_H