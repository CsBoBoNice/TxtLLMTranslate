#ifndef MDTRANSLATOR_H
#define MDTRANSLATOR_H

#include "FileTranslator.h"
#include "MdParser.h"
#include "MdPrompt.h"
#include "OpenaiManager.h"
#include <QPlainTextEdit>
#include <QString>
#include <QTime>
#include <QVector>

class MdTranslator : public FileTranslator
{
public:
    MdTranslator(int maxLen = 3072, int minLen = 1024);
    ~MdTranslator();

    // 实现父类的虚函数
    void setLog(QPlainTextEdit *logOutput) override
    {
        m_logOutput = logOutput;
    }
    bool translate(const QString &inputFilePath, const QString &outputFilePath,
                   const QString &url, const QString &apiKey, const QString &model,
                   bool keepHistory = false) override;

private:
    // 实现父类的虚函数
    void outlog(const QString &log) override;

    // 构建完整的翻译提示
    QString buildPrompt(const QString &content, const PromptInfo &promptInfo);

    MdParser m_mdParser;
    OpenaiManager *m_openaiManager;
    MdPrompt *m_translatePrompt;

    // 用于保存最近的翻译历史
    struct TranslateHistory
    {
        QString original;   // 原文
        QString translated; // 译文
    };
    QVector<TranslateHistory> m_history;

    // 段落长度限制
    int m_maxLen; // 最大段落长度
    int m_minLen; // 最小段落长度
};

#endif // MDTRANSLATOR_H 