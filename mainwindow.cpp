#include "mainwindow.h"
#include <QCoreApplication>
#include <QDebug>
#include <QSettings>
#include <QVBoxLayout>
#include <QGroupBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    m_settingsPath = QCoreApplication::applicationDirPath() + "/aiservicesettings.ini";

    initUI();
    loadSettings();
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::initUI()
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    setCentralWidget(centralWidget);

    m_tabWidget = new QTabWidget(this);
    mainLayout->addWidget(m_tabWidget);

    m_settingsTab = new SettingsTab(this);
    m_translationTab = new TranslationTab(this);

    m_tabWidget->addTab(m_translationTab, "翻译界面");
    m_tabWidget->addTab(m_settingsTab, "AI服务设置");

    QGroupBox *logGroup = new QGroupBox("运行日志", this);
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);

    m_logText = new QPlainTextEdit(this);
    m_logText->setReadOnly(true);
    logLayout->addWidget(m_logText);

    mainLayout->addWidget(logGroup);

    m_settingsTab->setLog(m_logText);
    m_translationTab->setLog(m_logText);

    connect(m_settingsTab, &SettingsTab::logMessage, this, &MainWindow::handleLogMessage);
    connect(m_translationTab, &TranslationTab::logMessage, this, &MainWindow::handleLogMessage);
    connect(m_translationTab, &TranslationTab::requestTranslation, this, &MainWindow::handleTranslationRequest);

    setWindowTitle("翻译工具");
    resize(800, 800);
}

void MainWindow::loadSettings()
{
    m_settingsTab->loadSettings(m_settingsPath);
}

void MainWindow::saveSettings()
{
    m_settingsTab->saveSettings(m_settingsPath);
}

void MainWindow::handleLogMessage(const QString &message)
{
    qDebug() << message;
    m_logText->appendPlainText(message);
}

void MainWindow::handleTranslationRequest()
{
    QString url    = m_settingsTab->getUrl();
    QString apiKey = m_settingsTab->getApiKey();
    QString model  = m_settingsTab->getModel();

    m_translationTab->startTranslate(url, apiKey, model);
}
