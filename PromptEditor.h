#ifndef PROMPTEDITOR_H
#define PROMPTEDITOR_H

#include "FilePrompt.h"
#include <QDialog>
#include <QtWidgets>

class PromptEditor : public QDialog
{
    Q_OBJECT

public:
    explicit PromptEditor(FilePrompt *filePrompt, QWidget *parent = nullptr);
    ~PromptEditor();

private slots:
    void savePrompt();
    void loadPrompt();
    void resetPrompt();
    void addUserPrompt();
    void editUserPrompt();
    void removeUserPrompt();
    void addAssistantPrompt();
    void editAssistantPrompt();
    void removeAssistantPrompt();

private:
    void initUI();
    void initConnections();
    void updateDisplay(const PromptInfo &info);

    QTextEdit *m_promptEdit;
    QTextEdit *m_prefixEdit;
    QTextEdit *m_suffixEdit;

    QListWidget *m_userPromptsList;
    QListWidget *m_assistantPromptsList;

    QPushButton *m_addUserPromptBtn;
    QPushButton *m_editUserPromptBtn;
    QPushButton *m_removeUserPromptBtn;

    QPushButton *m_addAssistantPromptBtn;
    QPushButton *m_editAssistantPromptBtn;
    QPushButton *m_removeAssistantPromptBtn;

    QPushButton *m_saveButton;
    QPushButton *m_cancelButton;
    QPushButton *m_resetButton;

    FilePrompt *m_filePrompt;
    QString m_jsonPath;
};

#endif // PROMPTEDITOR_H