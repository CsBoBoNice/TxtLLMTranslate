#ifndef PROMPTINFO_H
#define PROMPTINFO_H

#include <QString>
#include <QVector>

// 提示信息结构体
struct PromptInfo
{
    QString prompt;                       // 翻译提示
    QString chatPrefix;                   // 聊天提示前缀
    QString chatSuffix;                   // 聊天提示后缀
    QVector<QString> userPromptList;      // 用户提示列表
    QVector<QString> assistantPromptList; // 助手提示列表
};

#endif // PROMPTINFO_H