#include "RstPrompt.h"
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

RstPrompt *RstPrompt::instance = nullptr;

RstPrompt *RstPrompt::getInstance()
{
    if (instance == nullptr)
    {
        instance = new RstPrompt();
    }
    return instance;
}

RstPrompt::RstPrompt()
{
    qDebug() << "初始化RST翻译提示管理器";
    initDefaultPrompt();

    // 尝试从默认路径加载配置
    QString defaultPath = getPromptPath();
    if (!load(defaultPath))
    {
        qDebug() << "未能从默认路径加载配置，使用默认设置";
    }

    qDebug() << "已设置翻译提示信息";
}

QString RstPrompt::getPromptPath()
{
    return QCoreApplication::applicationDirPath() + "/rst_prompt.json";
}

void RstPrompt::initDefaultPrompt()
{
    // 设置默认值
    m_promptInfo.prompt = R"(
- Role: RST文档翻译专家
- Background: 用户需要将reStructuredText (RST) 格式文档翻译成简体中文，必须严格保持所有RST语法标记和文档结构，特别注意语法标记前后的空格处理。
- Profile: 你是一位精通RST的翻译专家，擅长技术文档翻译，对RST语法规则和空格要求有深入理解。
- Skills: 
  * RST语法精通
  * 技术文档翻译能力
  * 专业术语准确性把控
  * 文档结构维护能力
  * 空格规范处理能力
  * 交叉引用处理能力
- Goals: 
  * 准确翻译文档内容
  * 完整保留RST语法结构
  * 确保语法标记前后空格正确
  * 维护交叉引用完整性
  * 保持文档可读性和美观
- Constraints: 
  * 严格遵守RST语法规则
  * 确保以下语法标记前后必须有空格：
    - 星号强调: " *文本* "
    - 双星号加粗: " **文本** "
    - 反引号代码: " ``代码`` "
    - 超链接: " `链接文本`_ "
    - 引用标记: " :ref:`标签` "
  * 保持所有指令块格式
  * 维护标题层级标记
  * 保持代码块原样不译
  * 不改变任何链接路径
- OutputFormat: 输出标准RST格式的翻译内容，保持所有原有的格式标记和空格
- Workflow:
  1. 分析文档结构和RST语法特征
  2. 识别需要特殊空格处理的语法标记：
     * 行内标记（*、**、``、`_）
     * 角色标记（:role:`text`）
     * 引用标记（:ref:）
     * 指令标记（.. directive::）
  3. 翻译文本内容时：
     * 保持语法标记前后空格
     * 确保专业术语准确
     * 维护文档结构
  4. 检查格式完整性：
     * 验证所有标记的空格
     * 确认标题标记格式
     * 检查引用完整性
  5. 最终校对：
     * 确保语言流畅
     * 验证格式正确
     * 复查空格规范
- Examples:
  原文格式 -> 译文格式（注意空格）：
  * "*text*" -> " *文本* "
  * "**text**" -> " **文本** "
  * "``code``" -> " ``代码`` "
  * "`link`_" -> " `链接`_ "
  * ":ref:`label`" -> " :ref:`标签` "
  * ":doc:`text`" -> " :doc:`文本` "
  * ".. note::" -> ".. note::"（指令保持不变）
  * ".. warning::" -> ".. warning::"（指令保持不变）
  * ".. code-block::" -> ".. code-block::"（代码块保持不变）
)";

    m_promptInfo.chatPrefix = "[待翻译内容开始]";
    m_promptInfo.chatSuffix =
        "[待翻译内容结束]\n开始将待翻译内容翻译成简体中文，仅输出翻译内容，无需其他解释说明。";

    // 设置默认的用户提示列表
    m_promptInfo.userPromptList = {
        R"(
.. _dt-from-c:

Devicetree access from C/C++
############################

This guide describes Zephyr's ``<zephyr/devicetree.h>`` API for reading the
devicetree from C source files. It assumes you're familiar with the concepts in
:ref:`devicetree-intro` and :ref:`dt-bindings`. See :ref:`dt-reference` for
reference material.

A note for Linux developers
***************************

Linux developers familiar with devicetree should be warned that the API
described here differs significantly from how devicetree is used on Linux.

.. _dt-node-identifiers:
)",
        R"(
Node identifiers
****************

To get information about a particular devicetree node, you need a *node
identifier* for it. This is a just a C macro that refers to the node.

These are the main ways to get a node identifier:

By node label
   Use :c:func:`DT_NODELABEL()` to get a node identifier from a :ref:`node
   label <dt-node-labels>`. Node labels are often provided by SoC :file:`.dtsi`
   files to give nodes names that match the SoC datasheet, like ``i2c1``,
   ``spi2``, etc.

By alias
   Use :c:func:`DT_ALIAS()` to get a node identifier for a property of the
   special ``/aliases`` node. This is sometimes done by applications (like
   :zephyr:code-sample:`blinky`, which uses the ``led0`` alias) that need to
   refer to *some* device of a particular type ("the board's user LED") but
   don't care which one is used.

Two node identifiers which refer to the same node are identical and can be used
interchangeably.
)",
        R"(
.. _dt-node-main-ex:

Here's a DTS fragment for some imaginary hardware we'll return to throughout
this file for examples:

.. literalinclude:: main-example.dts
   :language: devicetree
   :start-after: start-after-here

Here are a few ways to get node identifiers for the ``i2c@40002000`` node:

- ``DT_PATH(soc, i2c_40002000)``
- ``DT_NODELABEL(i2c1)``
- ``DT_ALIAS(sensor_controller)``
- ``DT_INST(x, vnd_soc_i2c)`` for some unknown number ``x``. See the
  :c:func:`DT_INST()` documentation for details.

.. important::

   Non-alphanumeric characters like dash (``-``) and the at sign (``@``) in
   devicetree names are converted to underscores (``_``). The names in a DTS
   are also converted to lowercase.
)"};

    // 设置默认的助手提示列表
    m_promptInfo.assistantPromptList = {
        R"(
.. _dt-from-c:

从C/C++访问设备树
##############################

本指南描述了 Zephyr 的 ``<zephyr/devicetree.h>`` API，用于从 C 源文件中读取设备树。它假设您熟悉 :ref:`devicetree-intro` 和 :ref:`dt-bindings` 中的概念。有关参考资料，请参阅 :ref:`dt-reference` 。


Linux开发者注意事项
**********************************

熟悉设备树的Linux开发者应该注意，这里描述的API与Linux中设备树的使用方法存在显著差异。

.. _dt-node-identifiers:
)",
        R"(
节点标识符
*********************

要获取特定设备树节点信息，您需要一个 *节点标识符* 。这只是一个指向该节点的C宏。

以下是获取节点标识符的主要方法：

通过节点标签
   使用 :c:func:`DT_NODELABEL()` 从 :ref:`节点标签 <dt-node-labels>` 获取节点标识符。节点标签通常由SoC :file:`.dtsi` 文件提供，以便节点名称与SoC数据表匹配，如 ``i2c1`` 、 ``spi2`` 等等。

通过别名
   使用 :c:func:`DT_ALIAS()` 获得特殊 ``/aliases`` 节点的属性节点标识符。这通常由应用程序（如 :zephyr:code-sample:`blinky` ，它使用 ``led0`` 别名）完成，这些应用程序需要引用特定类型的 *某些* 设备（例如“板上的用户LED”）但不在乎使用哪个。

两个指向同一节点的节点标识符是相同的，可以互换使用。
)",
        R"(
.. _dt-node-main-ex:

以下是一个我们将在此文件中多次引用的假设硬件的DTS片段：

.. literalinclude:: main-example.dts
   :language: devicetree
   :start-after: start-after-here

以下是获取 ``i2c@40002000`` 节点节点标识符的几种方法：

- ``DT_PATH(soc, i2c_40002000)``
- ``DT_NODELABEL(i2c1)``
- ``DT_ALIAS(sensor_controller)``
- ``DT_INST(x, vnd_soc_i2c)`` 对于某些未知的数字 ``x`` 。有关详细信息，请参阅 :c:func:`DT_INST()` 文档。

.. important::

   设备树名称中的非字母数字字符，如连字符 ( ``-`` ) 和at符号 ( ``@`` ) ，将转换为下划线 ( ``_`` ) 。DTS中的名称也转换为小写。
)"};
}

RstPrompt::~RstPrompt() {}

bool RstPrompt::save(const QString &filePath)
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

bool RstPrompt::load(const QString &filePath)
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

void RstPrompt::changePrompt(const QString &prompt, const QString &chatPrefix,
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

void RstPrompt::resetToDefault()
{
    qDebug() << "重置为默认提示信息";
    initDefaultPrompt();
    qDebug() << "已恢复默认提示信息";
}