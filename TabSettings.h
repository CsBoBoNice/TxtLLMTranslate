#ifndef _TAB_SETTINGS_H_
#define _TAB_SETTINGS_H_

#include "llamaServerManager.h"
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

class SettingsTab : public QWidget
{
    Q_OBJECT
public:
    explicit SettingsTab(QWidget *parent = nullptr);
    void loadSettings(const QString &settingsPath);
    void saveSettings(const QString &settingsPath);
    void setDefaultValues();

    // getter方法
    QString getApiKey() const
    {
        return m_apiKeyEdit->text();
    }
    QString getUrl() const
    {
        return m_urlEdit->text();
    }
    QString getModel() const
    {
        return m_modelEdit->text();
    }

signals:
    void logMessage(const QString &message);

private slots:
    void onLocalAIToggled(bool checked);
    void selectModelFile();
    void testApiConnection();

private:
    void createUI();

    QLineEdit *m_apiKeyEdit;
    QLineEdit *m_urlEdit;
    QLineEdit *m_modelEdit;
    QPushButton *m_testButton;
    QLineEdit *m_testResult;

    QCheckBox *m_useLocalAICheck;
    QLineEdit *m_modelPathEdit;
    QLineEdit *m_hostEdit;
    QLineEdit *m_portEdit;
    QLineEdit *m_tempEdit;
    QLineEdit *m_contextSizeEdit;
    QLineEdit *m_nglEdit;
    QPushButton *m_selectModelButton;
    QPushButton *m_resetButton;

    LlamaServerManager *m_llamaServer;
    QTimer *m_checkServerTimer;
};

#endif // _TAB_SETTINGS_H_