#include "TabTranslation.h"
#include "prompteditor.h"
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QScrollBar>
#include <QVBoxLayout>

TranslationTab::TranslationTab(QWidget *parent) : QWidget(parent)
{
    createUI();
}

void TranslationTab::createUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 文件选择区域
    QGroupBox *fileGroup    = new QGroupBox("字幕文件", this);
    QHBoxLayout *fileLayout = new QHBoxLayout(fileGroup);

    m_srtPathEdit = new QLineEdit(this);
    m_srtPathEdit->setPlaceholderText("选择SRT字幕文件");
    m_selectButton = new QPushButton("浏览", this);

    fileLayout->addWidget(m_srtPathEdit);
    fileLayout->addWidget(m_selectButton);

    mainLayout->addWidget(fileGroup);

    // 字幕显示区域
    QGroupBox *textGroup    = new QGroupBox("字幕内容", this);
    QHBoxLayout *textLayout = new QHBoxLayout(textGroup);

    m_originalText = new QTextEdit(this);
    m_originalText->setReadOnly(true);
    m_translatedText = new QTextEdit(this);
    m_translatedText->setReadOnly(true);

    textLayout->addWidget(m_originalText);
    textLayout->addWidget(m_translatedText);

    // 添加滚动条同步
    connect(m_originalText->verticalScrollBar(), &QScrollBar::valueChanged,
            m_translatedText->verticalScrollBar(), &QScrollBar::setValue);
    connect(m_translatedText->verticalScrollBar(), &QScrollBar::valueChanged,
            m_originalText->verticalScrollBar(), &QScrollBar::setValue);

    mainLayout->addWidget(textGroup);

    // 操作按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_keepHistoryCheck        = new QCheckBox("保留历史记录用于参考", this);
    m_keepHistoryCheck->setChecked(true);
    m_translateButton  = new QPushButton("开始翻译", this);
    m_editPromptButton = new QPushButton("编辑提示", this);

    buttonLayout->addWidget(m_keepHistoryCheck);
    buttonLayout->addWidget(m_editPromptButton);
    buttonLayout->addWidget(m_translateButton);

    mainLayout->addLayout(buttonLayout);

    // 日志显示区域
    QGroupBox *logGroup    = new QGroupBox("运行日志", this);
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);

    m_logText = new QPlainTextEdit(this);
    m_logText->setReadOnly(true);

    logLayout->addWidget(m_logText);
    mainLayout->addWidget(logGroup);

    // 连接信号槽
    connect(m_selectButton, &QPushButton::clicked, this, &TranslationTab::selectSrtFile);
    connect(m_translateButton, &QPushButton::clicked, this, &TranslationTab::onTranslateClicked);
    connect(m_editPromptButton, &QPushButton::clicked, this,
            [this]()
            {
                PromptEditor editor(this);
                editor.exec();
            });

    // 设置翻译器的日志输出
    m_translator.setLog(m_logText);
}

void TranslationTab::selectSrtFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, "选择SRT字幕文件", QString(),
                                                    "字幕文件 (*.srt);;所有文件 (*.*)");

    if (!filePath.isEmpty())
    {
        m_srtPathEdit->setText(filePath);
        emit logMessage("已选择字幕文件: " + filePath);

        // 读取并显示原文
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file);
            in.setEncoding(QStringConverter::Utf8);
            m_originalText->setText(in.readAll());
            file.close();
        }
    }
}

void TranslationTab::startTranslate(const QString &url, const QString &apiKey, const QString &model)
{
    QString filePath = m_srtPathEdit->text();

    if (filePath.isEmpty())
    {
        QMessageBox::warning(this, "警告", "请先选择字幕文件");
        return;
    }

    emit logMessage("开始翻译字幕...");
    m_translateButton->setEnabled(false);

    bool keepHistory = m_keepHistoryCheck->isChecked();
    bool success     = m_translator.translate(filePath, url, apiKey, model, keepHistory);

    if (success)
    {
        emit logMessage("翻译完成");
        // 显示翻译结果
        QString translatedPath = filePath;
        translatedPath.replace(".srt", "_translated.srt");
        QFile file(translatedPath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file);
            in.setEncoding(QStringConverter::Utf8);
            m_translatedText->setText(in.readAll());
            file.close();
        }
    }
    else
    {
        emit logMessage("翻译失败");
    }

    m_translateButton->setEnabled(true);
}

void TranslationTab::updateLog(const QString &log)
{
    m_logText->appendPlainText(log);
    emit logMessage(log);
}

// 添加新的槽函数
void TranslationTab::onTranslateClicked()
{
    emit requestTranslation();
}

// 其余实现从MainWindow中移植相关方法