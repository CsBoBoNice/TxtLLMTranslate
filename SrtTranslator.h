#ifndef SRTTRANSLATOR_H
#define SRTTRANSLATOR_H

#include <QString>
#include "srtparser.h"
#include "StrPrompt.h"
#include "OpenaiManager.h"
#include <QPlainTextEdit>
#include <QTime>
#include <QVector>
#include "FileTranslator.h"

class SrtTranslator : public FileTranslator
{
public:
    SrtTranslator();
    ~SrtTranslator();
    
    // 实现父类的虚函数
    void setLog(QPlainTextEdit* logOutput) override { m_logOutput = logOutput; }
    bool translate(const QString& inputFilePath,
                  const QString& outputFilePath,
                  const QString& url,
                  const QString& apiKey,
                  const QString& model,
                  bool keepHistory = false) override;

private:
    // 实现父类的虚函数
    void outlog(const QString& log) override;
    
    // 构建完整的翻译提示
    QString buildPrompt(const QString& content, const PromptInfo& promptInfo);
    
    SrtParser m_srtParser;
    OpenaiManager* m_openaiManager;
    StrPrompt* m_translatePrompt;
    
    // 用于保存最近的翻译历史
    struct TranslateHistory {
        QString original;    // 原文
        QString translated;  // 译文
    };
    QVector<TranslateHistory> m_history;
};

#endif // SRTTRANSLATOR_H 