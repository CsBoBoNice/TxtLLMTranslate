#include "TxtPrompt.h"
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

TxtPrompt *TxtPrompt::instance = nullptr;

TxtPrompt *TxtPrompt::getInstance()
{
    if (instance == nullptr)
    {
        instance = new TxtPrompt();
    }
    return instance;
}

TxtPrompt::TxtPrompt()
{
    qDebug() << "初始化TXT翻译提示管理器";
    initDefaultPrompt();

    // 尝试从默认路径加载配置
    QString defaultPath = getPromptPath();
    if (!load(defaultPath))
    {
        qDebug() << "未能从默认路径加载配置，使用默认设置";
    }

    qDebug() << "已设置翻译提示信息";
}

QString TxtPrompt::getPromptPath()
{
    return QCoreApplication::applicationDirPath() + "/txt_prompt.json";
}

void TxtPrompt::initDefaultPrompt()
{
    // 设置默认值
    m_promptInfo.prompt =
        R"(
- Role: 专业翻译员
- Background: 用户需要将待翻译内容翻译成简体中文，并且需要保持文档的原始格式和专业术语的准确性。
- Profile: 你是一位经验丰富的翻译专家，精通简体中文翻译，擅长技术文档的翻译工作。
- Skills: 语言翻译能力、文档格式理解、专业术语准确性把控。
- Goals: 确保文档翻译后，格式、结构、链接和图片标记保持原样，同时专业术语翻译准确无误。
- Constrains: 翻译过程中，不得更改文档的任何非文本内容，包括链接、图片标记等。
- OutputFormat: 翻译后的文档保持格式。翻译后仅输出翻译内容，无需其他解释说明。
- Workflow:
  1. 阅读并理解原文本内容及其格式。
  2. 翻译文本内容，同时保留所有格式化指令。
  3. 检查翻译后的文档，确保链接和图片标记未被更改，专业术语准确。
- Initialization: 欢迎来到文档翻译服务，我将帮助您将待翻译内容翻译成简体中文，同时保留文档结构。请发送您需要翻译的文档。
)";
    m_promptInfo.chatPrefix = "[待翻译内容开始]";
    m_promptInfo.chatSuffix =
        "[待翻译内容结束]\n开始将待翻译内容翻译成简体中文，仅输出翻译内容，无需其他解释说明。";

    // 设置默认的用户提示列表
    m_promptInfo.userPromptList = {R"(
GIF decoder
Allow using GIF images in LVGL. Based on https://github.com/lecram/gifdec

When enabled in `lv_conf.h` with `LV_USE_GIF` `lv_gif_create(parent)` can be used to create a gif widget.

`lv_gif_set_src(obj, src)` works very similarly to `lv_img_set_src`. As source, it also accepts images as variables (`lv_img_dsc_t`) or files.


Convert GIF files to C array
To convert a GIF file to byte values array use [LVGL's online converter](https://lvgl.io/tools/imageconverter). Select "Raw" color format and "C array" Output format.


Use GIF images from file
For example:
```c
lv_gif_set_src(obj, "S:path/to/example.gif");
```

Note that, a file system driver needs to be registered to open images from files. Read more about it [here](https://docs.lvgl.io/master/overview/file-system.html) or just enable one in `lv_conf.h` with `LV_USE_FS_...`

)"};

    // 设置默认的助手提示列表
    m_promptInfo.assistantPromptList = {R"(
GIF解码器

允许在LVGL中使用GIF图像。基于 https://github.com/lecram/gifdec

在`lv_conf.h`中启用`LV_USE_GIF`后，可以使用`lv_gif_create(parent)`来创建一个GIF控件。

`lv_gif_set_src(obj, src)`与`lv_img_set_src`非常相似。它也接受变量（`lv_img_dsc_t`）或文件作为图像源。

将GIF文件转换为C数组

要将GIF文件转换为字节数组，请使用[LVGL在线转换器](https://lvgl.io/tools/imageconverter)。选择“Raw”颜色格式和“C array”输出格式。

从文件中使用GIF图像

例如：
```c
lv_gif_set_src(obj, "S:path/to/example.gif");
```

请注意，需要注册文件系统驱动程序才能从文件中打开图像。更多信息请阅读[这里](https://docs.lvgl.io/master/overview/file-system.html)，或者在`lv_conf.h`中启用`LV_USE_FS_...`

)"};
}

TxtPrompt::~TxtPrompt() {}

bool TxtPrompt::save(const QString &filePath)
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

bool TxtPrompt::load(const QString &filePath)
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

void TxtPrompt::changePrompt(const QString &prompt, const QString &chatPrefix,
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

void TxtPrompt::resetToDefault()
{
    qDebug() << "重置为默认提示信息";
    initDefaultPrompt();
    qDebug() << "已恢复默认提示信息";
}