#include "TabSettings.h"
#include "openaimanager.h"
#include <QCoreApplication>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSettings>
#include <QSysInfo>
#include <QTimer>
#include <QVBoxLayout>

SettingsTab::SettingsTab(QWidget *parent)
    : QWidget(parent), m_checkServerTimer(nullptr), m_llamaServer(new LlamaServerManager(this))
{
    createUI();
    setDefaultValues();
}

void SettingsTab::createUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // OpenAI设置区域
    QGroupBox *apiGroup    = new QGroupBox("OpenAI设置", this);
    QGridLayout *apiLayout = new QGridLayout(apiGroup);

    m_apiKeyEdit = new QLineEdit(this);
    m_apiKeyEdit->setPlaceholderText("API Key");
    m_urlEdit    = new QLineEdit(this);
    m_modelEdit  = new QLineEdit(this);
    m_testButton = new QPushButton("测试连接", this);
    m_testResult = new QLineEdit(this);
    m_testResult->setReadOnly(true);

    apiLayout->addWidget(new QLabel("API Key:", this), 0, 0);
    apiLayout->addWidget(m_apiKeyEdit, 0, 1);
    apiLayout->addWidget(new QLabel("URL:", this), 1, 0);
    apiLayout->addWidget(m_urlEdit, 1, 1);
    apiLayout->addWidget(new QLabel("Model:", this), 2, 0);
    apiLayout->addWidget(m_modelEdit, 2, 1);
    apiLayout->addWidget(m_testButton, 3, 0);
    apiLayout->addWidget(m_testResult, 3, 1);

    mainLayout->addWidget(apiGroup);

    // 本地AI服务设置区域
    QGroupBox *localAIGroup    = new QGroupBox("本地AI服务设置", this);
    QGridLayout *localAILayout = new QGridLayout(localAIGroup);

    m_useLocalAICheck = new QCheckBox("使用本地AI服务", this);
    m_modelPathEdit   = new QLineEdit(this);
    m_modelPathEdit->setPlaceholderText("AI模型文件路径");
    m_selectModelButton = new QPushButton("选择模型", this);
    m_hostEdit          = new QLineEdit(this);
    m_portEdit          = new QLineEdit(this);
    m_tempEdit          = new QLineEdit(this);
    m_contextSizeEdit   = new QLineEdit(this);
    m_nglEdit           = new QLineEdit(this);

    localAILayout->addWidget(m_useLocalAICheck, 0, 0, 1, 2);
    localAILayout->addWidget(new QLabel("模型路径:", this), 1, 0);
    QHBoxLayout *modelPathLayout = new QHBoxLayout();
    modelPathLayout->addWidget(m_modelPathEdit);
    modelPathLayout->addWidget(m_selectModelButton);
    localAILayout->addLayout(modelPathLayout, 1, 1);
    localAILayout->addWidget(new QLabel("主机地址:", this), 2, 0);
    localAILayout->addWidget(m_hostEdit, 2, 1);
    localAILayout->addWidget(new QLabel("端口:", this), 3, 0);
    localAILayout->addWidget(m_portEdit, 3, 1);
    localAILayout->addWidget(new QLabel("温度:", this), 4, 0);
    localAILayout->addWidget(m_tempEdit, 4, 1);
    localAILayout->addWidget(new QLabel("上下文大小:", this), 5, 0);
    localAILayout->addWidget(m_contextSizeEdit, 5, 1);
    localAILayout->addWidget(new QLabel("NGL:", this), 6, 0);
    localAILayout->addWidget(m_nglEdit, 6, 1);

    mainLayout->addWidget(localAIGroup);

    // 添加恢复默认设置按钮
    m_resetButton = new QPushButton("恢复默认设置", this);
    mainLayout->addWidget(m_resetButton);

    // 连接信号槽
    connect(m_useLocalAICheck, &QCheckBox::toggled, this, &SettingsTab::onLocalAIToggled);
    connect(m_selectModelButton, &QPushButton::clicked, this, &SettingsTab::selectModelFile);
    connect(m_testButton, &QPushButton::clicked, this, &SettingsTab::testApiConnection);
    connect(m_resetButton, &QPushButton::clicked, this,
            [this]()
            {
                if (QMessageBox::question(this, "确认", "确定要恢复默认设置吗？") ==
                    QMessageBox::Yes)
                {
                    setDefaultValues();
                    emit logMessage("已恢复默认设置");
                }
            });

    mainLayout->addStretch();
}

void SettingsTab::setDefaultValues()
{
    m_urlEdit->setText("http://127.0.0.1:11434/v1/chat/completions");
    m_apiKeyEdit->setText("123456");
    m_modelEdit->setText("qwen2.5:3b-instruct-q8_0");

    m_useLocalAICheck->setChecked(false);
    m_modelPathEdit->setText(QCoreApplication::applicationDirPath() + "/model/model.GGUF");
    m_hostEdit->setText("127.0.0.1");
    m_portEdit->setText("11434");
    m_tempEdit->setText("0.7");
    m_contextSizeEdit->setText("2048");
    m_nglEdit->setText("100");
}

void SettingsTab::loadSettings(const QString &settingsPath)
{
    QSettings settings(settingsPath, QSettings::IniFormat);

    m_urlEdit->setText(settings.value("OpenAI/url", m_urlEdit->text()).toString());
    m_apiKeyEdit->setText(settings.value("OpenAI/apiKey", m_apiKeyEdit->text()).toString());
    m_modelEdit->setText(settings.value("OpenAI/model", m_modelEdit->text()).toString());

    m_modelPathEdit->setText(
        settings.value("LocalAI/modelPath", m_modelPathEdit->text()).toString());
    m_hostEdit->setText(settings.value("LocalAI/host", m_hostEdit->text()).toString());
    m_portEdit->setText(settings.value("LocalAI/port", m_portEdit->text()).toString());
    m_tempEdit->setText(settings.value("LocalAI/temp", m_tempEdit->text()).toString());
    m_contextSizeEdit->setText(
        settings.value("LocalAI/contextSize", m_contextSizeEdit->text()).toString());
    m_nglEdit->setText(settings.value("LocalAI/ngl", m_nglEdit->text()).toString());
}

void SettingsTab::saveSettings(const QString &settingsPath)
{
    QSettings settings(settingsPath, QSettings::IniFormat);

    settings.setValue("OpenAI/url", m_urlEdit->text());
    settings.setValue("OpenAI/apiKey", m_apiKeyEdit->text());
    settings.setValue("OpenAI/model", m_modelEdit->text());

    settings.setValue("LocalAI/modelPath", m_modelPathEdit->text());
    settings.setValue("LocalAI/host", m_hostEdit->text());
    settings.setValue("LocalAI/port", m_portEdit->text());
    settings.setValue("LocalAI/temp", m_tempEdit->text());
    settings.setValue("LocalAI/contextSize", m_contextSizeEdit->text());
    settings.setValue("LocalAI/ngl", m_nglEdit->text());

    settings.sync();
}

void SettingsTab::onLocalAIToggled(bool checked)
{
    if (checked)
    {
        QString programPath;
        if (QSysInfo::productType() == "windows")
        {
            programPath = QCoreApplication::applicationDirPath() + "/llama_bin/llama-server.exe";
        }
        else
        {
            programPath = QCoreApplication::applicationDirPath() + "/llama_bin/llama-server";
        }

        bool success = m_llamaServer->startServer(
            programPath, m_modelPathEdit->text(), m_hostEdit->text(), m_portEdit->text(),
            m_tempEdit->text(), m_contextSizeEdit->text(), m_nglEdit->text());

        if (!success)
        {
            emit logMessage("本地AI服务启动失败");
            QMessageBox::critical(this, "错误", "本地AI服务启动失败，请检查设置和程序路径");
            m_useLocalAICheck->setChecked(false);
            return;
        }

        if (m_checkServerTimer)
        {
            delete m_checkServerTimer;
        }
        m_checkServerTimer = new QTimer(this);
        m_checkServerTimer->setSingleShot(true);
        connect(m_checkServerTimer, &QTimer::timeout, this,
                [this]()
                {
                    if (!m_llamaServer->isRunning())
                    {
                        emit logMessage("本地AI服务启动错误");
                        QMessageBox::critical(this, "错误",
                                              "本地AI服务启动错误，请检查设置和程序路径");
                        m_useLocalAICheck->setChecked(false);
                        m_llamaServer->stopServer();
                    }
                    else
                    {
                        emit logMessage("本地AI服务已成功启动并运行");
                        QMessageBox::information(this, "成功", "本地AI服务已启动");
                    }
                });
        m_checkServerTimer->start(1000);
    }
    else
    {
        m_llamaServer->stopServer();
        emit logMessage("本地AI服务已停止");
    }
}

void SettingsTab::selectModelFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, "选择AI模型文件", m_modelPathEdit->text(),
                                                    "GGUF模型文件 (*.gguf);;所有文件 (*.*)");

    if (!filePath.isEmpty())
    {
        m_modelPathEdit->setText(filePath);
        emit logMessage("已选择模型文件: " + filePath);
    }
}

void SettingsTab::testApiConnection()
{
    QString apiKey = m_apiKeyEdit->text();
    QString url    = m_urlEdit->text();
    QString model  = m_modelEdit->text();

    emit logMessage("正在测试API连接...");

    OpenaiManager *manager = OpenaiManager::getInstance();
    bool success           = manager->testApi(url, apiKey, model);

    if (success)
    {
        m_testResult->setText("连接成功");
        m_testResult->setStyleSheet("color: green;");
        emit logMessage("API连接测试成功");
    }
    else
    {
        m_testResult->setText("连接失败");
        m_testResult->setStyleSheet("color: red;");
        emit logMessage("API连接测试失败");
    }
}

// 其余实现从MainWindow中移植相关方法