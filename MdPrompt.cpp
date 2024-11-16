#include "MdPrompt.h"
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

MdPrompt *MdPrompt::instance = nullptr;

MdPrompt *MdPrompt::getInstance()
{
    if (instance == nullptr)
    {
        instance = new MdPrompt();
    }
    return instance;
}

MdPrompt::MdPrompt()
{
    qDebug() << "初始化MD翻译提示管理器";
    initDefaultPrompt();

    // 尝试从默认路径加载配置
    QString defaultPath = getPromptPath();
    if (!load(defaultPath))
    {
        qDebug() << "未能从默认路径加载配置，使用默认设置";
    }

    qDebug() << "已设置翻译提示信息";
}

QString MdPrompt::getPromptPath()
{
    return QCoreApplication::applicationDirPath() + "/md_prompt.json";
}

void MdPrompt::initDefaultPrompt()
{
    // 设置默认值
    m_promptInfo.prompt     = R"(
- Role: 专业翻译员
- Background: 用户需要将待翻译内容翻译成简体中文，并且需要保持翻译内容原始格式和专业术语的准确性。
- Profile: 你是一位经验丰富的翻译专家，精通简体中文翻译，擅长技术文档的翻译工作。
- Skills: 语言翻译能力、文档格式理解、专业术语准确性把控。
- Goals: 确保文档翻译后，格式、结构、链接和图片标记保持原样，同时专业术语翻译准确无误。
- Constrains: 翻译过程中，不得更改文档的任何非文本内容，包括链接、图片标记等。
- OutputFormat: 翻译后的文档以Markdown格式输出。翻译后仅输出翻译内容 ，无需其他解释说明。
- Workflow:
  1. 阅读并理解原文本内容及其格式。
  2. 翻译文本内容，同时保留所有格式化指令。
  3. 检查翻译后的文档，确保链接和图片标记未被更改，专业术语准确。
- Initialization: 欢迎来到文档翻译服务，我将帮助您将待翻译内容翻译成简体中文，同时保留所有格式化指令和文档结构。请发送您需要翻译的文档。
)";
    m_promptInfo.chatPrefix = "[待翻译内容开始]";
    m_promptInfo.chatSuffix =
        "[待翻译内容结束]\n开始将待翻译内容翻译成简体中文，仅输出翻译内容，无需其他解释说明。";

    // 设置默认的用户提示列表
    m_promptInfo.userPromptList = {
        R"(
## Memory requirements
To decode and display a GIF animation the following amount of RAM is required:
- `LV_COLOR_DEPTH 8`: 3 x image width x image height
- `LV_COLOR_DEPTH 16`: 4 x image width x image height
- `LV_COLOR_DEPTH 32`: 5 x image width x image height

## Example
```eval_rst
.. include:: ../../examples/libs/gif/index.rst
```

## API

```eval_rst
.. doxygenfile:: lv_gif.h
  :project: lvgl
```
)"};

    // 设置默认的助手提示列表
    m_promptInfo.assistantPromptList = {
        R"(
## 内存要求

为了解码和显示GIF动画，需要以下数量的RAM：
- `LV_COLOR_DEPTH 8`: 3 x 图像宽度 x 图像高度
- `LV_COLOR_DEPTH 16`: 4 x 图像宽度 x 图像高度
- `LV_COLOR_DEPTH 32`: 5 x 图像宽度 x 图像高度

## 示例

```eval_rst
.. include:: ../../examples/libs/gif/index.rst
```

## API

```eval_rst
.. doxygenfile:: lv_gif.h
  :project: lvgl
```
)"};
}

MdPrompt::~MdPrompt() {}

bool MdPrompt::save(const QString &filePath)
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

bool MdPrompt::load(const QString &filePath)
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

void MdPrompt::changePrompt(const QString &prompt, const QString &chatPrefix,
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

void MdPrompt::resetToDefault()
{
    qDebug() << "重置为默认提示信息";
    initDefaultPrompt();
    qDebug() << "已恢复默认提示信息";
}