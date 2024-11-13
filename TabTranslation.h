#ifndef _TAB_TRANSLATION_H
#define _TAB_TRANSLATION_H

#include "srttranslator.h"
#include <QCheckBox>
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
    void selectSrtFile();
    void onTranslateClicked();

private:
    void createUI();
    void updateLog(const QString &log);

    QLineEdit *m_srtPathEdit;
    QPushButton *m_selectButton;
    QTextEdit *m_originalText;
    QTextEdit *m_translatedText;
    QPushButton *m_translateButton;
    QPushButton *m_editPromptButton;
    QPlainTextEdit *m_logText;
    QCheckBox *m_keepHistoryCheck;

    SrtTranslator m_translator;
};

#endif // _TAB_TRANSLATION_H