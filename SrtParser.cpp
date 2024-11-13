#include "SrtParser.h"
#include <QDebug>
#include <QFile>
#include <QRegularExpression>
#include <QTextStream>

SrtParser::SrtParser()
{
    qDebug() << "初始化SRT解析器";
}

SrtParser::~SrtParser() {}

bool SrtParser::parse(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "无法打开SRT文件:" << filePath;
        return false;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8); // 使用新的编码设置方式

    m_srtInfoList.clear();
    SrtInfo currentInfo;
    int state = 0; // 0:序号 1:时间 2:内容

    QString line;
    while (!in.atEnd())
    {
        line = in.readLine().trimmed();

        switch (state)
        {
        case 0: // 序号
            if (!line.isEmpty())
            {
                bool ok;
                line.toInt(&ok);
                if (!ok)
                {
                    qDebug() << "无效的SRT序号行:" << line;
                    file.close();
                    return false;
                }
                state = 1;
            }
            break;

        case 1: // 时间
            if (!line.isEmpty())
            {
                if (!parseTimeString(line, currentInfo.time))
                {
                    qDebug() << "无效的时间格式:" << line;
                    file.close();
                    return false;
                }
                state = 2;
                currentInfo.content.clear();
            }
            break;

        case 2: // 内容
            if (line.isEmpty())
            {
                if (!currentInfo.content.isEmpty() && !currentInfo.time.isEmpty())
                {
                    m_srtInfoList.append(currentInfo);
                    currentInfo = SrtInfo(); // 重置当前字幕信息
                    state       = 0;
                }
            }
            else
            {
                if (!currentInfo.content.isEmpty())
                {
                    currentInfo.content += "\n";
                }
                currentInfo.content += line;
            }
            break;
        }
    }

    // 处理最后一条字幕：只有当状态为2（正在处理内容）且内容和时间都不为空时才添加
    if (state == 2 && !currentInfo.content.isEmpty() && !currentInfo.time.isEmpty())
    {
        m_srtInfoList.append(currentInfo);
    }

    file.close();
    qDebug() << "成功解析SRT文件，共" << m_srtInfoList.size() << "条字幕";
    return true;
}

bool SrtParser::save(const QString &filePath, const QVector<SrtInfo> &srtInfoList)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "无法创建SRT文件:" << filePath;
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8); // 使用新的编码设置方式

    for (int i = 0; i < srtInfoList.size(); ++i)
    {
        // 写入序号
        out << (i + 1) << "\n";

        // 写入时间
        out << srtInfoList[i].time << "\n";

        // 只写入翻译内容
        if (!srtInfoList[i].translateContent.isEmpty())
        {
            out << srtInfoList[i].translateContent;
        }
        else
        {
            // 如果没有翻译内容，则写入原文
            out << srtInfoList[i].content;
        }

        // 写入空行
        out << "\n\n";
    }

    file.close();
    qDebug() << "成功保存SRT文件:" << filePath;
    return true;
}

bool SrtParser::parseTimeString(const QString &timeStr, QString &time)
{
    // 时间格式：00:00:00,000 --> 00:00:00,000
    QRegularExpression timeRegex(
        "([0-9]{2}:[0-9]{2}:[0-9]{2},[0-9]{3}) --> ([0-9]{2}:[0-9]{2}:[0-9]{2},[0-9]{3})");

    QRegularExpressionMatch match = timeRegex.match(timeStr);
    if (!match.hasMatch())
    {
        return false;
    }

    time = timeStr;
    return true;
}