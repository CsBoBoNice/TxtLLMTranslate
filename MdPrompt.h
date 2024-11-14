#ifndef MDPROMPT_H
#define MDPROMPT_H

#include "FilePrompt.h"
#include "PromptInfo.h"
#include <QCoreApplication>
#include <QString>
#include <QVector>

class MdPrompt : public FilePrompt
{
public:
    // 获取单例实例
    static MdPrompt *getInstance();

    // 实现父类的虚函数
    PromptInfo getPrompt() const override
    {
        return m_promptInfo;
    }
    bool save(const QString &filePath) override;
    bool load(const QString &filePath) override;
    void changePrompt(const QString &prompt, const QString &chatPrefix, const QString &chatSuffix,
                      const QVector<QString> &userPromptList,
                      const QVector<QString> &assistantPromptList) override;
    void resetToDefault() override;
    QString getPromptPath() override;

private:
    MdPrompt();
    ~MdPrompt();

    // 禁用拷贝构造和赋值操作
    MdPrompt(const MdPrompt &) = delete;
    MdPrompt &operator=(const MdPrompt &) = delete;

    // 初始化默认提示信息
    void initDefaultPrompt();

    static MdPrompt *instance;
};

#endif // MDPROMPT_H 