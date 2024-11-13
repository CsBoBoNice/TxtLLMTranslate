#ifndef SRTTRANSLATOR_H
#define SRTTRANSLATOR_H

#include <QString>
#include "srtparser.h"
#include "StrPrompt.h"
#include "OpenaiManager.h"
#include <QPlainTextEdit>
#include <QTime>
#include <QVector>

class SrtTranslator
{
public:
    SrtTranslator();
    ~SrtTranslator();
    
    // 设置日志输出控件
    void setLog(QPlainTextEdit* logOutput) { m_logOutput = logOutput; }
    
    // 翻译字幕文件
    bool translate(const QString& filePath, 
                  const QString& url, 
                  const QString& apiKey,
                  const QString& model,
                  bool keepHistory = false);

private:
    // 输出日志
    void outlog(const QString& log);
    
    // 构建完整的翻译提示
    QString buildPrompt(const QString& content, const PromptInfo& promptInfo);
    
    SrtParser m_srtParser;
    OpenaiManager* m_openaiManager;
    StrPrompt* m_translatePrompt;
    QPlainTextEdit* m_logOutput = nullptr;  // 日志输出控件
    
    // 用于保存最近的翻译历史
    struct TranslateHistory {
        QString original;    // 原文
        QString translated;  // 译文
    };
    QVector<TranslateHistory> m_history;  // 新增成员变量
};

#endif // SRTTRANSLATOR_H 