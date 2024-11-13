#ifndef _TAB_TRANSLATION_H
#define _TAB_TRANSLATION_H

#include "FileManager.h"
#include "FileTranslator.h"
#include "TxtTranslator.h"
#include "SrtTranslator.h"
#include <QCheckBox>
#include <QComboBox>
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

private:
    void createUI();
    void updateLog(const QString &log);

    // UI控件
    QLineEdit *m_inputPathEdit;      // 输入路径编辑框
    QLineEdit *m_outputPathEdit;     // 输出路径编辑框
    QPushButton *m_inputSelectBtn;   // 输入路径选择按钮
    QPushButton *m_outputSelectBtn;  // 输出路径选择按钮
    QTextEdit *m_translatedText;     // 翻译内容显示
    QPushButton *m_translateButton;  // 翻译按钮
    QComboBox *m_fileTypeCombo;     // 文件类型选择下拉框
    QPushButton *m_editPromptButton; // 编辑提示按钮
    QPlainTextEdit *m_logText;      // 日志显示
    QCheckBox *m_keepHistoryCheck;   // 保留历史记录复选框

    // 文件管理器
    FileManager m_fileManager;
};

#endif // _TAB_TRANSLATION_H