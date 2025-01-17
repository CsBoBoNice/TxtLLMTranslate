#include "SrtPrompt.h"
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

SrtPrompt *SrtPrompt::instance = nullptr;

SrtPrompt *SrtPrompt::getInstance()
{
    if (instance == nullptr)
    {
        instance = new SrtPrompt();
    }
    return instance;
}

SrtPrompt::SrtPrompt()
{
    qDebug() << "初始化翻译提示管理器";
    initDefaultPrompt();

    // 尝试从默认路径加载配置
    QString defaultPath = getPromptPath();
    if (!load(defaultPath))
    {
        qDebug() << "未能从默认路径加载配置，使用默认设置";
    }

    qDebug() << "已设置翻译提示信息";
}

QString SrtPrompt::getPromptPath()
{
    return QCoreApplication::applicationDirPath() + "/srt_prompt.json";
}

void SrtPrompt::initDefaultPrompt()
{
    // 设置默认值
    m_promptInfo.prompt = R"(
- Role: 专业字幕翻译员
- Background: 用户需要将SRT字幕文件翻译成简体中文，需要理解上下文并保持翻译的连贯性。
- Profile: 你是一位专业的字幕翻译专家，擅长影视内容翻译，注重字幕的简洁性、可读性和上下文连贯性。
- Skills: 
  * 精准的字幕翻译能力
  * 口语化表达转换
  * 上下文关系理解
  * 人物语气把握
  * 场景氛围传达
- Goals: 
  * 准确传达对话内容和说话语气
  * 保持字幕简洁易读
  * 保持对话的连贯性和上下文关系
  * 准确传达场景氛围和情感
- Constraints: 
  * 保持字幕的分行结构
  * 不添加任何额外注释
  * 保持人物对话风格的一致性
  * 注意代词和指代的准确性
- OutputFormat: 只输出翻译内容，无需其他解释说明
- Workflow:
  1. 阅读完整对话内容，理解上下文关系
  2. 识别说话人物的语气和风格特点
  3. 结合场景理解对话的深层含义
  4. 翻译时保持人物对话风格
  5. 确保前后文的连贯性
  6. 调整译文长度和分行
  7. 确保字幕的可读性
)";
    m_promptInfo.chatPrefix = "[待翻译内容开始]";
    m_promptInfo.chatSuffix = "[待翻译内容结束]\n开始将待翻译内容翻译成简体中文。";

    // 设置默认的用户提示列表
    m_promptInfo.userPromptList = {"春の風が吹き抜けるとき、心は新緑に染まる。",
                                   "未来は明るく、希望に満ちている。",
                                   "歩むたびに、新しい道が開ける。"};

    // 设置默认的助手提示列表
    m_promptInfo.assistantPromptList = {"春天吹拂的风拂来时，心灵便会化身为绿意盎然的新叶。",
                                        "未来很光明，充满着希望。",
                                        "每次往前踏出一步，崭新的道路就会开启。"};
}

SrtPrompt::~SrtPrompt() {}

bool SrtPrompt::save(const QString &filePath)
{
    QJsonObject jsonObject;

    // 保存基本字段
    jsonObject["prompt"]     = m_promptInfo.prompt;
    jsonObject["chatPrefix"] = m_promptInfo.chatPrefix;
    jsonObject["chatSuffix"] = m_promptInfo.chatSuffix;

    // 保存用户提示列表
    QJsonArray userPrompts;
    for (const QString &prompt : m_promptInfo.userPromptList)
    {
        userPrompts.append(prompt);
    }
    jsonObject["userPrompts"] = userPrompts;

    // 保存助手提示列表
    QJsonArray assistantPrompts;
    for (const QString &prompt : m_promptInfo.assistantPromptList)
    {
        assistantPrompts.append(prompt);
    }
    jsonObject["assistantPrompts"] = assistantPrompts;

    // 写入文件
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "无法创建JSON文件:" << filePath;
        return false;
    }

    QJsonDocument jsonDoc(jsonObject);
    file.write(jsonDoc.toJson());
    file.close();

    qDebug() << "成功保存提示信息到文件:" << filePath;
    return true;
}

bool SrtPrompt::load(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "无法打开JSON文件:" << filePath;
        return false;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isNull())
    {
        qDebug() << "JSON文件格式无效:" << filePath;
        return false;
    }

    QJsonObject jsonObject = jsonDoc.object();

    // 读取基本字段
    if (jsonObject.contains("prompt"))
    {
        m_promptInfo.prompt = jsonObject["prompt"].toString();
    }
    if (jsonObject.contains("chatPrefix"))
    {
        m_promptInfo.chatPrefix = jsonObject["chatPrefix"].toString();
    }
    if (jsonObject.contains("chatSuffix"))
    {
        m_promptInfo.chatSuffix = jsonObject["chatSuffix"].toString();
    }

    // 读取用户提示列表
    m_promptInfo.userPromptList.clear();
    if (jsonObject.contains("userPrompts") && jsonObject["userPrompts"].isArray())
    {
        QJsonArray userPrompts = jsonObject["userPrompts"].toArray();
        for (const QJsonValue &value : userPrompts)
        {
            m_promptInfo.userPromptList.append(value.toString());
        }
    }

    // 读取助手提示列表
    m_promptInfo.assistantPromptList.clear();
    if (jsonObject.contains("assistantPrompts") && jsonObject["assistantPrompts"].isArray())
    {
        QJsonArray assistantPrompts = jsonObject["assistantPrompts"].toArray();
        for (const QJsonValue &value : assistantPrompts)
        {
            m_promptInfo.assistantPromptList.append(value.toString());
        }
    }

    qDebug() << "成功从文件加载提示信息:" << filePath;
    return true;
}

void SrtPrompt::changePrompt(const QString &prompt, const QString &chatPrefix,
                             const QString &chatSuffix, const QVector<QString> &userPromptList,
                             const QVector<QString> &assistantPromptList)
{
    qDebug() << "修改提示信息";

    // 更新提示信息
    m_promptInfo.prompt              = prompt;
    m_promptInfo.chatPrefix          = chatPrefix;
    m_promptInfo.chatSuffix          = chatSuffix;
    m_promptInfo.userPromptList      = userPromptList;
    m_promptInfo.assistantPromptList = assistantPromptList;

    qDebug() << "提示信息已更新";
}

void SrtPrompt::resetToDefault()
{
    qDebug() << "重置为默认提示信息";
    initDefaultPrompt();
    qDebug() << "已恢复默认提示信息";
}
