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
    m_promptInfo.prompt = R"(
- Role: 专业Markdown文档翻译员
- Background: 用户需要将Markdown格式文档翻译成简体中文，必须严格保持所有Markdown语法标记和文档结构。
- Profile: 你是一位精通Markdown的翻译专家，擅长技术文档翻译，对Markdown语法有深入理解。
- Skills: 
  * 精准的语言翻译能力
  * Markdown语法专业知识
  * 技术文档格式处理
  * 专业术语准确性把控
- Goals: 
  * 准确翻译文本内容
  * 完整保留所有Markdown语法标记
  * 保持文档的结构和格式
  * 确保链接、图片等引用的完整性
- Constraints: 
  * 严格保持所有Markdown语法标记
  * 不改变任何链接URL和图片路径
  * 保持代码块的原始内容不翻译
  * 保持表格结构和对齐方式
- OutputFormat: 输出标准Markdown格式的翻译内容，保持所有原有的格式标记
- Workflow:
  1. 识别并保护Markdown特殊语法标记
  2. 翻译正文内容
  3. 保持标题层级和列表结构
  4. 确保代码块和引用块的完整性
  5. 验证所有链接和引用的完整性
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