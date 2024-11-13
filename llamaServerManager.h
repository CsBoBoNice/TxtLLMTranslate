#ifndef LLAMASERVERMANAGER_H
#define LLAMASERVERMANAGER_H

#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>

class LlamaServerManager : public QObject
{
    Q_OBJECT

public:
    explicit LlamaServerManager(QObject *parent = nullptr);
    ~LlamaServerManager();

    // 启动llama-server服务，返回是否启动成功
    bool startServer(const QString &program, const QString &model, const QString &host,
                     const QString &port, const QString &temp, const QString &contextSize,
                     const QString &ngl);

    // 停止llama-server服务
    void stopServer();

    // 获取服务是否正在运行
    bool isRunning() const
    {
        return m_process->state() != QProcess::NotRunning;
    }

private slots:
    void handleOutput();
    void handleError(QProcess::ProcessError error);
    void handleFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QProcess *m_process;
};

#endif // LLAMASERVERMANAGER_H
