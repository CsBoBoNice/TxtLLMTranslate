#ifndef TRANSLATEPROMPT_H
#define TRANSLATEPROMPT_H

#include <QString>
#include <QVector>
#include "PromptInfo.h"
#include <QCoreApplication>
#include "FilePrompt.h"

class StrPrompt : public FilePrompt
{
public:
    // 获取单例实例
    static StrPrompt* getInstance();
    
    // 实现父类的虚函数
    PromptInfo getPrompt() const override { return m_promptInfo; }
    bool save(const QString& filePath) override;
    bool load(const QString& filePath) override;
    void changePrompt(const QString& prompt,
                     const QString& chatPrefix,
                     const QString& chatSuffix,
                     const QVector<QString>& userPromptList,
                     const QVector<QString>& assistantPromptList) override;
    void resetToDefault() override;
    QString getPromptPath() override;

private:
    StrPrompt();
    ~StrPrompt();
    
    // 禁用拷贝构造和赋值操作
    StrPrompt(const StrPrompt&) = delete;
    StrPrompt& operator=(const StrPrompt&) = delete;
    
    // 初始化默认提示信息
    void initDefaultPrompt();
    
    static StrPrompt* instance;
};

#endif // TRANSLATEPROMPT_H 