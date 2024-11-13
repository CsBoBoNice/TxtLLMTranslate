#include "PromptEditor.h"
#include <QtWidgets>

PromptEditor::PromptEditor(FilePrompt* filePrompt, QWidget *parent)
    : QDialog(parent)
    , m_filePrompt(filePrompt)
{
    m_jsonPath = m_filePrompt->getPromptPath();
    initUI();
    initConnections();
    loadPrompt();
}

PromptEditor::~PromptEditor()
{
}

void PromptEditor::initUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 基本设置部分修改
    QLabel* promptLabel = new QLabel("翻译提示:", this);
    m_promptEdit = new QTextEdit(this);
    m_promptEdit->setAcceptRichText(false);  // 只接受纯文本
    m_promptEdit->setMinimumHeight(60);      // 设置最小高度
    mainLayout->addWidget(promptLabel);
    mainLayout->addWidget(m_promptEdit);

    QLabel* prefixLabel = new QLabel("聊天前缀:", this);
    m_prefixEdit = new QTextEdit(this);
    m_prefixEdit->setAcceptRichText(false);
    m_prefixEdit->setMinimumHeight(60);
    mainLayout->addWidget(prefixLabel);
    mainLayout->addWidget(m_prefixEdit);

    QLabel* suffixLabel = new QLabel("聊天后缀:", this);
    m_suffixEdit = new QTextEdit(this);
    m_suffixEdit->setAcceptRichText(false);
    m_suffixEdit->setMinimumHeight(60);
    mainLayout->addWidget(suffixLabel);
    mainLayout->addWidget(m_suffixEdit);

    // 用户提示列表
    QGroupBox* userGroup = new QGroupBox("用户提示列表", this);
    QVBoxLayout* userLayout = new QVBoxLayout(userGroup);
    
    m_userPromptsList = new QListWidget(this);
    userLayout->addWidget(m_userPromptsList);

    QHBoxLayout* userBtnLayout = new QHBoxLayout;
    m_addUserPromptBtn = new QPushButton("添加", this);
    m_editUserPromptBtn = new QPushButton("编辑", this);
    m_removeUserPromptBtn = new QPushButton("删除", this);
    userBtnLayout->addWidget(m_addUserPromptBtn);
    userBtnLayout->addWidget(m_editUserPromptBtn);
    userBtnLayout->addWidget(m_removeUserPromptBtn);
    userLayout->addLayout(userBtnLayout);
    
    mainLayout->addWidget(userGroup);

    // 助手提示列表
    QGroupBox* assistantGroup = new QGroupBox("助手提示列表", this);
    QVBoxLayout* assistantLayout = new QVBoxLayout(assistantGroup);
    
    m_assistantPromptsList = new QListWidget(this);
    assistantLayout->addWidget(m_assistantPromptsList);

    QHBoxLayout* assistantBtnLayout = new QHBoxLayout;
    m_addAssistantPromptBtn = new QPushButton("添加", this);
    m_editAssistantPromptBtn = new QPushButton("编辑", this);
    m_removeAssistantPromptBtn = new QPushButton("删除", this);
    assistantBtnLayout->addWidget(m_addAssistantPromptBtn);
    assistantBtnLayout->addWidget(m_editAssistantPromptBtn);
    assistantBtnLayout->addWidget(m_removeAssistantPromptBtn);
    assistantLayout->addLayout(assistantBtnLayout);
    
    mainLayout->addWidget(assistantGroup);

    // 底部按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_resetButton = new QPushButton("恢复默认", this);
    m_saveButton = new QPushButton("保存", this);
    m_cancelButton = new QPushButton("取消", this);
    buttonLayout->addWidget(m_resetButton);
    buttonLayout->addWidget(m_saveButton);
    buttonLayout->addWidget(m_cancelButton);
    mainLayout->addLayout(buttonLayout);

    setWindowTitle("编辑翻译提示");
    resize(500, 700);
}

void PromptEditor::initConnections()
{
    connect(m_addUserPromptBtn, &QPushButton::clicked, this, &PromptEditor::addUserPrompt);
    connect(m_editUserPromptBtn, &QPushButton::clicked, this, &PromptEditor::editUserPrompt);
    connect(m_removeUserPromptBtn, &QPushButton::clicked, this, &PromptEditor::removeUserPrompt);
    
    connect(m_addAssistantPromptBtn, &QPushButton::clicked, this, &PromptEditor::addAssistantPrompt);
    connect(m_editAssistantPromptBtn, &QPushButton::clicked, this, &PromptEditor::editAssistantPrompt);
    connect(m_removeAssistantPromptBtn, &QPushButton::clicked, this, &PromptEditor::removeAssistantPrompt);
    
    connect(m_saveButton, &QPushButton::clicked, this, &PromptEditor::savePrompt);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_resetButton, &QPushButton::clicked, this, &PromptEditor::resetPrompt);
}

void PromptEditor::addUserPrompt()
{
    bool ok;
    QString text = QInputDialog::getMultiLineText(this, "添加用户提示",
                                                "请输入提示内容:", "", &ok);
    if (ok && !text.isEmpty()) {
        m_userPromptsList->addItem(text);
    }
}

void PromptEditor::editUserPrompt()
{
    QListWidgetItem* current = m_userPromptsList->currentItem();
    if (!current) return;
    
    bool ok;
    QString text = QInputDialog::getMultiLineText(this, "编辑用户提示",
                                                "请输入提示内容:", current->text(), &ok);
    if (ok && !text.isEmpty()) {
        current->setText(text);
    }
}

void PromptEditor::removeUserPrompt()
{
    QListWidgetItem* current = m_userPromptsList->currentItem();
    if (current) {
        delete m_userPromptsList->takeItem(m_userPromptsList->row(current));
    }
}

void PromptEditor::addAssistantPrompt()
{
    bool ok;
    QString text = QInputDialog::getMultiLineText(this, "添加助手提示",
                                                "请输入提示内容:", "", &ok);
    if (ok && !text.isEmpty()) {
        m_assistantPromptsList->addItem(text);
    }
}

void PromptEditor::editAssistantPrompt()
{
    QListWidgetItem* current = m_assistantPromptsList->currentItem();
    if (!current) return;
    
    bool ok;
    QString text = QInputDialog::getMultiLineText(this, "编辑助手提示",
                                                "请输入提示内容:", current->text(), &ok);
    if (ok && !text.isEmpty()) {
        current->setText(text);
    }
}

void PromptEditor::removeAssistantPrompt()
{
    QListWidgetItem* current = m_assistantPromptsList->currentItem();
    if (current) {
        delete m_assistantPromptsList->takeItem(m_assistantPromptsList->row(current));
    }
}

void PromptEditor::savePrompt()
{
    QString prompt = m_promptEdit->toPlainText();
    QString prefix = m_prefixEdit->toPlainText();
    QString suffix = m_suffixEdit->toPlainText();

    QVector<QString> userPrompts;
    for (int i = 0; i < m_userPromptsList->count(); ++i) {
        userPrompts.append(m_userPromptsList->item(i)->text());
    }

    QVector<QString> assistantPrompts;
    for (int i = 0; i < m_assistantPromptsList->count(); ++i) {
        assistantPrompts.append(m_assistantPromptsList->item(i)->text());
    }

    m_filePrompt->changePrompt(prompt, prefix, suffix, userPrompts, assistantPrompts);

    if (m_filePrompt->save(m_jsonPath)) {
        QMessageBox::information(this, "成功", "提示信息已保存");
        accept();
    } else {
        QMessageBox::warning(this, "错误", "保存提示信息失败");
    }
}

void PromptEditor::updateDisplay(const PromptInfo& info)
{
    m_promptEdit->setPlainText(info.prompt);     // 使用 setPlainText() 设置文本
    m_prefixEdit->setPlainText(info.chatPrefix); // 使用 setPlainText() 设置文本
    m_suffixEdit->setPlainText(info.chatSuffix); // 使用 setPlainText() 设置文本

    m_userPromptsList->clear();
    for (const QString& prompt : info.userPromptList) {
        m_userPromptsList->addItem(prompt);
    }

    m_assistantPromptsList->clear();
    for (const QString& prompt : info.assistantPromptList) {
        m_assistantPromptsList->addItem(prompt);
    }
}

void PromptEditor::loadPrompt()
{
    m_filePrompt->load(m_jsonPath);
    updateDisplay(m_filePrompt->getPrompt());
}

void PromptEditor::resetPrompt()
{
    if (QMessageBox::question(this, "确认", "确定要恢复默认提示吗？",
                            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        m_filePrompt->resetToDefault();
        updateDisplay(m_filePrompt->getPrompt());
        QMessageBox::information(this, "成功", "已恢复默认提示");
    }
} 