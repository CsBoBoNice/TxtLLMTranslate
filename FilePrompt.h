#ifndef FILEPROMPT_H
#define FILEPROMPT_H

#include "PromptInfo.h"
#include <QString>
#include <QVector>

class FilePrompt
{
public:
    // 获取提示信息
    virtual PromptInfo getPrompt() const = 0;

    // 保存提示信息到文件
    virtual bool save(const QString &filePath) = 0;

    // 从文件加载提示信息
    virtual bool load(const QString &filePath) = 0;

    // 修改提示信息
    virtual void changePrompt(const QString &prompt, const QString &chatPrefix,
                              const QString &chatSuffix, const QVector<QString> &userPromptList,
                              const QVector<QString> &assistantPromptList) = 0;

    // 恢复默认值
    virtual void resetToDefault() = 0;

    // 获取默认存储路径
    virtual QString getPromptPath() = 0;

    // 虚析构函数
    virtual ~FilePrompt() = default;

protected:
    FilePrompt() = default;
    PromptInfo m_promptInfo;
};

#endif // FILEPROMPT_H