#ifndef TRANSLATEPROMPT_H
#define TRANSLATEPROMPT_H

#include <QString>
#include <QVector>
#include "PromptInfo.h"
#include <QCoreApplication>

class StrPrompt
{
public:
    // 获取单例实例
    static StrPrompt* getInstance();
    
    // 获取提示信息
    PromptInfo getPrompt() const { return m_promptInfo; }
    
    // 保存提示信息到JSON文件
    bool save(const QString& filePath);
    
    // 从JSON文件加载提示信息
    bool load(const QString& filePath);
    
    // 修改提示信息
    void changePrompt(const QString& prompt,
                     const QString& chatPrefix,
                     const QString& chatSuffix,
                     const QVector<QString>& userPromptList,
                     const QVector<QString>& assistantPromptList);
    
    // 恢复默认值
    void resetToDefault();
    
    // 获取默认存储路径
    static QString getPromptPath();

private:
    StrPrompt();
    ~StrPrompt();
    
    // 禁用拷贝构造和赋值操作
    StrPrompt(const StrPrompt&) = delete;
    StrPrompt& operator=(const StrPrompt&) = delete;
    
    // 初始化默认提示信息
    void initDefaultPrompt();
    
    static StrPrompt* instance;
    PromptInfo m_promptInfo;
};

#endif // TRANSLATEPROMPT_H 