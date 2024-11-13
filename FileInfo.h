#ifndef FILEINFO_H
#define FILEINFO_H

#include <QList>
#include <QString>
#include <QVector>

// 文件类型枚举
enum class FileType
{
    TXT_FILE,
    MD_FILE,
    RST_FILE,
    SRT_FILE,
    OTHER
};

// 文件信息结构体
struct FileInfo
{
    QString fileName;                // 文件名
    QString filePath;                // 文件路径
    FileType fileType;               // 文件类型
};

#endif // FILEINFO_H
