#ifndef FILETRANSLATOR_H
#define FILETRANSLATOR_H

#include <QString>
#include <QPlainTextEdit>

class FileTranslator {
public:
    // 设置日志输出控件
    virtual void setLog(QPlainTextEdit* logOutput) = 0;
    
    // 翻译文件
    virtual bool translate(const QString& inputFilePath,
                         const QString& outputFilePath, 
                         const QString& url,
                         const QString& apiKey,
                         const QString& model,
                         bool keepHistory = false) = 0;
    
    // 虚析构函数
    virtual ~FileTranslator() = default;

protected:
    FileTranslator() = default;
    
    // 输出日志的虚函数
    virtual void outlog(const QString& log) = 0;
    
    QPlainTextEdit* m_logOutput = nullptr;
};

#endif // FILETRANSLATOR_H 