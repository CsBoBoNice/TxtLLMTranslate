#ifndef _TAB_TRANSLATION_H
#define _TAB_TRANSLATION_H

#include "FileManager.h"
#include "FileTranslator.h"
#include "SrtTranslator.h"
#include "TxtTranslator.h"
#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDir>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>

class TranslationTab : public QWidget
{
    Q_OBJECT
public:
    explicit TranslationTab(QWidget *parent = nullptr);

public slots:
    void startTranslate(const QString &url, const QString &apiKey, const QString &model);

signals:
    void logMessage(const QString &message);
    void requestTranslation();

private slots:
    void selectInputPath();
    void selectOutputPath();
    void onTranslateClicked();
    void onEditPromptClicked();
    void onSetParagraphLengthClicked();

private:
    void createUI();
    void updateLog(const QString &log);
    void loadParagraphSettings();
    void saveParagraphSettings();

    // UI控件
    QLineEdit *m_inputPathEdit;           // 输入路径编辑框
    QLineEdit *m_outputPathEdit;          // 输出路径编辑框
    QPushButton *m_inputSelectBtn;        // 输入路径选择按钮
    QPushButton *m_outputSelectBtn;       // 输出路径选择按钮
    QTextEdit *m_originalText;            // 原始内容显示
    QTextEdit *m_translatedText;          // 翻译内容显示
    QPushButton *m_translateButton;       // 翻译按钮
    QComboBox *m_fileTypeCombo;           // 文件类型选择下拉框
    QPushButton *m_editPromptButton;      // 编辑提示按钮
    QPlainTextEdit *m_logText;            // 日志显示
    QCheckBox *m_keepHistoryCheck;        // 保留历史记录复选框
    QPushButton *m_setParagraphLengthBtn; // 段落长度设置按钮

    // 文件管理器
    FileManager m_fileManager;

    // 段落长度限制
    int m_maxLen = 3072; // 最大段落长度
    int m_minLen = 1024; // 最小段落长度
};

#endif // _TAB_TRANSLATION_H