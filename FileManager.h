#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QString>
#include <QList>
#include "FileInfo.h"

class FileManager
{
public:
    FileManager();
    ~FileManager();

    // 遍历指定文件夹下的所有文件
    bool traverseDirectory(const QString& dirPath);

    // 获取文件列表
    QList<FileInfo>& getFiles();

private:
    // 判断文件类型
    FileType determineFileType(const QString& fileName);

    QList<FileInfo> m_files;    // 存储文件信息的列表
};

#endif // FILEMANAGER_H 