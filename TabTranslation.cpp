#include "TabTranslation.h"
#include "MdPrompt.h"
#include "PromptEditor.h"
#include "SrtPrompt.h"
#include "TxtPrompt.h"
#include <QDir>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QScrollBar>
#include <QSettings>
#include <QVBoxLayout>

TranslationTab::TranslationTab(QWidget *parent) : QWidget(parent)
{
    createUI();
    loadParagraphSettings();

    // 创建默认文件夹
    QString appPath = QCoreApplication::applicationDirPath();
    QDir dir;

    // 创建输入文件夹
    QString inputPath = appPath + "/inputDir";
    if (!dir.exists(inputPath))
    {
        if (dir.mkpath(inputPath))
        {
            emit logMessage("已创建输入文件夹: " + inputPath);
        }
        else
        {
            emit logMessage("创建输入文件夹失败: " + inputPath);
        }
    }

    // 创建输出文件夹
    QString outputPath = appPath + "/outputDir";
    if (!dir.exists(outputPath))
    {
        if (dir.mkpath(outputPath))
        {
            emit logMessage("已创建输出文件夹: " + outputPath);
        }
        else
        {
            emit logMessage("创建输出文件夹失败: " + outputPath);
        }
    }
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

    // 设置默认路径
    QString appPath = QCoreApplication::applicationDirPath();
    m_inputPathEdit->setText(appPath + "/inputDir");
    m_outputPathEdit->setText(appPath + "/outputDir");

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
    QHBoxLayout *contentLayout = new QHBoxLayout(contentGroup);

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

    m_setParagraphLengthBtn = new QPushButton("设置段落长度", this);
    m_editPromptButton      = new QPushButton("编辑提示", this);
    m_translateButton       = new QPushButton("开始翻译", this);

    buttonLayout->addWidget(m_keepHistoryCheck);
    buttonLayout->addWidget(m_fileTypeCombo);
    buttonLayout->addWidget(m_setParagraphLengthBtn);
    buttonLayout->addWidget(m_editPromptButton);
    buttonLayout->addWidget(m_translateButton);

    mainLayout->addLayout(buttonLayout);

    // 连接信号槽
    connect(m_inputSelectBtn, &QPushButton::clicked, this, &TranslationTab::selectInputPath);
    connect(m_outputSelectBtn, &QPushButton::clicked, this, &TranslationTab::selectOutputPath);
    connect(m_translateButton, &QPushButton::clicked, this, &TranslationTab::onTranslateClicked);
    connect(m_editPromptButton, &QPushButton::clicked, this, &TranslationTab::onEditPromptClicked);
    connect(m_setParagraphLengthBtn, &QPushButton::clicked, this,
            &TranslationTab::onSetParagraphLengthClicked);

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
    case FileType::MD_FILE:
        prompt = MdPrompt::getInstance();
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
    QString inputPath = m_inputPathEdit->text();
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
    m_originalText->clear();
    m_translatedText->clear();

    bool keepHistory = m_keepHistoryCheck->isChecked();
    QList<FileInfo> &files = m_fileManager.getFiles();

    for (const FileInfo &file : files)
    {
        FileTranslator *translator = nullptr;

        // 计算相对路径
        QString relativePath = QDir(inputPath).relativeFilePath(QFileInfo(file.filePath).path());
        // 构建输出文件的完整路径，保持目录结构
        QString outputDir = QDir(outputPath).absoluteFilePath(relativePath);
        
        // 确保输出目录存在
        QDir().mkpath(outputDir);
        
        QString outputFilePath = QDir(outputDir).absoluteFilePath(file.fileName);

        switch (file.fileType)
        {
            case FileType::TXT_FILE:
                translator = new TxtTranslator(m_maxLen, m_minLen);
                break;
            case FileType::SRT_FILE:
                translator = new SrtTranslator();
                break;
            case FileType::MD_FILE:
                translator = new MdTranslator(m_maxLen, m_minLen);
                break;
            default:
                // 处理不支持的文件类型
                emit logMessage("不支持的文件类型，将直接复制文件: " + file.fileName);
                // 确保目标目录存在
                QDir().mkpath(QFileInfo(outputFilePath).path());
                QFile::copy(file.filePath, outputFilePath);
                continue;
        }

        if (translator)
        {
            translator->setLog(m_logOutput);
            bool success = translator->translate(file.filePath, outputFilePath, url, apiKey, model,
                                                 keepHistory);

            // 读取并显示原文
            QFile originalFile(file.filePath);
            if (originalFile.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QTextStream in(&originalFile);
                in.setEncoding(QStringConverter::Utf8);
                m_originalText->clear();
                m_originalText->setText(in.readAll());
                originalFile.close();
            }

            // 显示翻译结果
            QFile resultFile(outputFilePath);
            if (resultFile.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QTextStream in(&resultFile);
                in.setEncoding(QStringConverter::Utf8);
                m_translatedText->clear();
                m_translatedText->setText(in.readAll());
                resultFile.close();
            }

            if (success)
            {
                emit logMessage("翻译成功");
            }
            else
            {
                emit logMessage("翻译失败！！！");
            }

            delete translator;
        }
    }

    m_translateButton->setEnabled(true);
}

void TranslationTab::updateLog(const QString &log)
{
    if (m_logOutput)
    {
        m_logOutput->appendPlainText(log);
    }
    emit logMessage(log);
}

void TranslationTab::onTranslateClicked()
{
    m_logOutput->clear();
    emit requestTranslation();
}

void TranslationTab::onSetParagraphLengthClicked()
{
    // 创建对话框
    QDialog dialog(this);
    dialog.setWindowTitle("设置段落长度");

    // 创建布局
    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    // 创建输入控件
    QFormLayout *formLayout = new QFormLayout();
    QSpinBox *maxLengthBox  = new QSpinBox(&dialog);
    QSpinBox *minLengthBox  = new QSpinBox(&dialog);

    // 设置SpinBox的范围
    maxLengthBox->setRange(1024, 4096);
    minLengthBox->setRange(64, 1024);

    // 使用当前保存的值作为初始值
    maxLengthBox->setValue(m_maxLen);
    minLengthBox->setValue(m_minLen);

    formLayout->addRow("最大长度:", maxLengthBox);
    formLayout->addRow("最小长度:", minLengthBox);
    layout->addLayout(formLayout);

    // 添加按钮
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    layout->addWidget(buttonBox);

    // 连接按钮信号
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    // 显示对话框
    if (dialog.exec() == QDialog::Accepted)
    {
        int maxLen = maxLengthBox->value();
        int minLen = minLengthBox->value();

        if (maxLen <= minLen)
        {
            QMessageBox::warning(this, "警告", "最大长度必须大于最小长度！");
            return;
        }

        // 更新段落长度限制
        m_maxLen = maxLen;
        m_minLen = minLen;

        // 保存设置
        saveParagraphSettings();

        emit logMessage(
            QString("设置段落长度 - 最大长度: %1, 最小长度: %2").arg(maxLen).arg(minLen));
    }
}

void TranslationTab::loadParagraphSettings()
{
    QString iniPath = QCoreApplication::applicationDirPath() + "/paragraph.ini";
    QSettings settings(iniPath, QSettings::IniFormat);

    m_maxLen = settings.value("Paragraph/MaxLength", 3072).toInt();
    m_minLen = settings.value("Paragraph/MinLength", 1024).toInt();
}

void TranslationTab::saveParagraphSettings()
{
    QString iniPath = QCoreApplication::applicationDirPath() + "/paragraph.ini";
    QSettings settings(iniPath, QSettings::IniFormat);

    settings.setValue("Paragraph/MaxLength", m_maxLen);
    settings.setValue("Paragraph/MinLength", m_minLen);
}