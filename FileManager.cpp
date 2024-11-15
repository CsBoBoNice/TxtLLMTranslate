#include "FileManager.h"
#include <QDebug>
#include <QDir>
#include <QDirIterator>

FileManager::FileManager() {}

FileManager::~FileManager() {}

bool FileManager::traverseDirectory(const QString &dirPath)
{
    try
    {
        // 清空之前的文件列表
        m_files.clear();

        QDir dir(dirPath);
        if (!dir.exists())
        {
            qDebug() << "错误：目录" << dirPath << "不存在";
            return false;
        }

        // 创建目录迭代器，递归遍历所有子目录
        QDirIterator it(dirPath, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

        while (it.hasNext())
        {
            QString filePath = it.next();
            QFileInfo fileInfo(filePath);

            // 创建FileInfo结构体并填充信息
            FileInfo info;
            info.fileName = fileInfo.fileName();
            info.filePath = fileInfo.absoluteFilePath();
            info.fileType = determineFileType(info.fileName);

            // 添加到文件列表
            m_files.append(info);

            qDebug() << "发现文件：" << info.fileName << "，路径：" << info.filePath;
        }

        qDebug() << "文件遍历完成，共发现" << m_files.size() << "个文件";
        return true;
    }
    catch (const std::exception &e)
    {
        qDebug() << "遍历目录时发生异常：" << e.what();
        return false;
    }
}

QList<FileInfo> &FileManager::getFiles()
{
    return m_files;
}

FileType FileManager::determineFileType(const QString &fileName)
{
    QString suffix = QFileInfo(fileName).suffix().toLower();

    if (suffix == "txt" || suffix == "TXT")
        return FileType::TXT_FILE;
    if (suffix == "md" || suffix == "MD")
        return FileType::MD_FILE;
    if (suffix == "rst" || suffix == "RST")
        return FileType::RST_FILE;
    if (suffix == "srt" || suffix == "SRT")
        return FileType::SRT_FILE;

    return FileType::OTHER;
}