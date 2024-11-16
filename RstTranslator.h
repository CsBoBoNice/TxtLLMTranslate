#ifndef RSTTRANSLATOR_H
#define RSTTRANSLATOR_H

#include "FileTranslator.h"
#include "RstParser.h"
#include "RstPrompt.h"
#include "OpenaiManager.h"
#include <QPlainTextEdit>
#include <QString>
#include <QTime>
#include <QVector>

class RstTranslator : public FileTranslator
{
public:
    RstTranslator(int maxLen = 3072, int minLen = 1024);
    ~RstTranslator();

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

    RstParser m_rstParser;
    OpenaiManager *m_openaiManager;
    RstPrompt *m_translatePrompt;

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

#endif // RSTTRANSLATOR_H 