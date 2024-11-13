#include "TabTranslation.h"
#include "PromptEditor.h"
#include "SrtPrompt.h"
#include "TxtPrompt.h"
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

    // 文件路径区域
    QGroupBox *pathGroup    = new QGroupBox("文件路径", this);
    QGridLayout *pathLayout = new QGridLayout(pathGroup);

    QLabel *inputLabel  = new QLabel("输入路径:", this);
    QLabel *outputLabel = new QLabel("输出路径:", this);

    m_inputPathEdit  = new QLineEdit(this);
    m_outputPathEdit = new QLineEdit(this);
    m_inputPathEdit->setPlaceholderText("选择待翻译文件所在文件夹");
    m_outputPathEdit->setPlaceholderText("选择翻译结果输出文件夹");

    m_inputSelectBtn  = new QPushButton("浏览", this);
    m_outputSelectBtn = new QPushButton("浏览", this);

    pathLayout->addWidget(inputLabel, 0, 0);
    pathLayout->addWidget(m_inputPathEdit, 0, 1);
    pathLayout->addWidget(m_inputSelectBtn, 0, 2);
    pathLayout->addWidget(outputLabel, 1, 0);
    pathLayout->addWidget(m_outputPathEdit, 1, 1);
    pathLayout->addWidget(m_outputSelectBtn, 1, 2);

    mainLayout->addWidget(pathGroup);

    // 翻译内容区域
    QGroupBox *contentGroup    = new QGroupBox("翻译内容", this);
    QVBoxLayout *contentLayout = new QVBoxLayout(contentGroup);

    m_originalText = new QTextEdit(this);
    m_originalText->setReadOnly(true);
    contentLayout->addWidget(m_originalText);

    m_translatedText = new QTextEdit(this);
    m_translatedText->setReadOnly(true);
    contentLayout->addWidget(m_translatedText);

    mainLayout->addWidget(contentGroup);

    // 操作按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    m_keepHistoryCheck = new QCheckBox("保留历史记录用于参考", this);
    m_keepHistoryCheck->setChecked(true);

    m_fileTypeCombo = new QComboBox(this);
    m_fileTypeCombo->addItem("TXT文件", static_cast<int>(FileType::TXT_FILE));
    m_fileTypeCombo->addItem("SRT字幕", static_cast<int>(FileType::SRT_FILE));
    m_fileTypeCombo->addItem("MD文档", static_cast<int>(FileType::MD_FILE));

    m_editPromptButton = new QPushButton("编辑提示", this);
    m_translateButton  = new QPushButton("开始翻译", this);

    buttonLayout->addWidget(m_keepHistoryCheck);
    buttonLayout->addWidget(m_fileTypeCombo);
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
    connect(m_inputSelectBtn, &QPushButton::clicked, this, &TranslationTab::selectInputPath);
    connect(m_outputSelectBtn, &QPushButton::clicked, this, &TranslationTab::selectOutputPath);
    connect(m_translateButton, &QPushButton::clicked, this, &TranslationTab::onTranslateClicked);
    connect(m_editPromptButton, &QPushButton::clicked, this, &TranslationTab::onEditPromptClicked);

    // 添加滚动条同步
    connect(m_originalText->verticalScrollBar(), &QScrollBar::valueChanged,
            m_translatedText->verticalScrollBar(), &QScrollBar::setValue);
    connect(m_translatedText->verticalScrollBar(), &QScrollBar::valueChanged,
            m_originalText->verticalScrollBar(), &QScrollBar::setValue);
}

void TranslationTab::selectInputPath()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, "选择输入文件夹");
    if (!dirPath.isEmpty())
    {
        m_inputPathEdit->setText(dirPath);
        emit logMessage("已选择输入文件夹: " + dirPath);
    }
}

void TranslationTab::selectOutputPath()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, "选择输出文件夹");
    if (!dirPath.isEmpty())
    {
        m_outputPathEdit->setText(dirPath);
        emit logMessage("已选择输出文件夹: " + dirPath);
    }
}

void TranslationTab::onEditPromptClicked()
{
    FilePrompt *prompt = nullptr;
    FileType type      = static_cast<FileType>(m_fileTypeCombo->currentData().toInt());

    switch (type)
    {
    case FileType::TXT_FILE:
        prompt = TxtPrompt::getInstance();
        break;
    case FileType::SRT_FILE:
        prompt = SrtPrompt::getInstance();
        break;
    default:
        QMessageBox::warning(this, "警告", "暂不支持该文件类型的提示词编辑");
        return;
    }

    if (prompt)
    {
        PromptEditor editor(prompt, this);
        editor.exec();
    }
}

void TranslationTab::startTranslate(const QString &url, const QString &apiKey, const QString &model)
{
    QString inputPath  = m_inputPathEdit->text();
    QString outputPath = m_outputPathEdit->text();

    if (inputPath.isEmpty() || outputPath.isEmpty())
    {
        QMessageBox::warning(this, "警告", "请先选择输入和输出文件夹");
        return;
    }

    // 遍历文件夹获取文件列表
    if (!m_fileManager.traverseDirectory(inputPath))
    {
        QMessageBox::warning(this, "错误", "遍历文件夹失败");
        return;
    }

    emit logMessage("开始翻译任务...");
    m_translateButton->setEnabled(false);
    m_translatedText->clear();

    bool keepHistory       = m_keepHistoryCheck->isChecked();
    QList<FileInfo> &files = m_fileManager.getFiles();

    for (const FileInfo &file : files)
    {
        FileTranslator *translator = nullptr;

        switch (file.fileType)
        {
        case FileType::TXT_FILE:
            translator = new TxtTranslator();
            break;
        case FileType::SRT_FILE:
            translator = new SrtTranslator();
            break;
        default:
            emit logMessage("跳过不支持的文件类型: " + file.fileName);
            continue;
        }

        // 读取并显示原文
        QFile originalFile(file.filePath);
        if (originalFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&originalFile);
            in.setEncoding(QStringConverter::Utf8);
            m_originalText->setText(in.readAll());
            originalFile.close();
        }

        QString outputFilePath = outputPath + "/" + file.fileName;

        if (translator)
        {
            translator->setLog(m_logText);
            bool success = translator->translate(file.filePath, outputFilePath, url, apiKey, model,
                                                 keepHistory);

            if (success)
            {
                // 显示翻译结果
                QFile resultFile(outputFilePath);
                if (resultFile.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    QTextStream in(&resultFile);
                    in.setEncoding(QStringConverter::Utf8);
                    m_translatedText->append("=== " + file.fileName + " ===\n");
                    m_translatedText->append(in.readAll() + "\n\n");
                    resultFile.close();
                }
            }

            delete translator;
        }
    }

    m_translateButton->setEnabled(true);
    emit logMessage("翻译任务完成");
}

void TranslationTab::updateLog(const QString &log)
{
    m_logText->appendPlainText(log);
    emit logMessage(log);
}

void TranslationTab::onTranslateClicked()
{
    emit requestTranslation();
}