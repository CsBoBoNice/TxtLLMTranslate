#ifndef TXTPROMPT_H
#define TXTPROMPT_H

#include <QString>
#include <QVector>
#include "PromptInfo.h"
#include <QCoreApplication>
#include "FilePrompt.h"

class TxtPrompt : public FilePrompt
{
public:
    // 获取单例实例
    static TxtPrompt* getInstance();
    
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
    TxtPrompt();
    ~TxtPrompt();
    
    // 禁用拷贝构造和赋值操作
    TxtPrompt(const TxtPrompt&) = delete;
    TxtPrompt& operator=(const TxtPrompt&) = delete;
    
    // 初始化默认提示信息
    void initDefaultPrompt();
    
    static TxtPrompt* instance;
};

#endif // TXTPROMPT_H 