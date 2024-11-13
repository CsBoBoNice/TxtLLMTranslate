#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "TabSettings.h"
#include "TabTranslation.h"
#include <QMainWindow>
#include <QTabWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleLogMessage(const QString &message);
    void handleTranslationRequest();

private:
    void initUI();
    void loadSettings();
    void saveSettings();

    QTabWidget *m_tabWidget;
    SettingsTab *m_settingsTab;
    TranslationTab *m_translationTab;
    QString m_settingsPath;
};

#endif // MAINWINDOW_H
