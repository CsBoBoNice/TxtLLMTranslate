#include "mainwindow.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 创建中心部件
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // 创建垂直布局
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    
    // 创建测试按钮
    m_testButton = new QPushButton("测试文件管理器", this);
    layout->addWidget(m_testButton);
    
    // 连接按钮点击信号到测试槽
    connect(m_testButton, &QPushButton::clicked, this, &MainWindow::testFileManager);
    
    // 设置窗口标题和大小
    setWindowTitle("文件管理器测试");
    resize(400, 300);
}

MainWindow::~MainWindow()
{
}

void MainWindow::testFileManager()
{
    // 打开文件夹选择对话框
    QString dirPath = QFileDialog::getExistingDirectory(
        this,
        "选择要遍历的文件夹",
        QDir::homePath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );
    
    if (dirPath.isEmpty()) {
        qDebug() << "未选择文件夹";
        return;
    }
    
    // 遍历选中的文件夹
    if (m_fileManager.traverseDirectory(dirPath)) {
        // 获取并显示文件列表
        QList<FileInfo> files = m_fileManager.getFiles();
        qDebug() << "文件列表获取成功，共" << files.size() << "个文件";
        
        // 显示每个文件的详细信息
        for (const FileInfo& file : files) {
            QString typeStr;
            switch (file.fileType) {
                case FileType::TXT_FILE: typeStr = "TXT"; break;
                case FileType::MD_FILE:  typeStr = "MD"; break;
                case FileType::RST_FILE: typeStr = "RST"; break;
                case FileType::SRT_FILE: typeStr = "SRT"; break;
                default: typeStr = "其他"; break;
            }
            
            qDebug() << "文件名:" << file.fileName
                     << "类型:" << typeStr
                     << "路径:" << file.filePath;
        }
    } else {
        qDebug() << "遍历文件夹失败";
    }
}
