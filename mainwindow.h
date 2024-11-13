#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include "FileManager.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    // 测试文件管理器功能
    void testFileManager();
    
    FileManager m_fileManager;        // 文件管理器实例
    QPushButton* m_testButton;        // 测试按钮
};
#endif // MAINWINDOW_H
