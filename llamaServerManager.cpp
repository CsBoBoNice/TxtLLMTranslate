#include "llamaServerManager.h"
#include <QDebug>
#include <QDir>

LlamaServerManager::LlamaServerManager(QObject *parent)
    : QObject(parent), m_process(new QProcess(this))
{
    connect(m_process, &QProcess::readyReadStandardOutput, this, &LlamaServerManager::handleOutput);
    connect(m_process, &QProcess::readyReadStandardError, this, &LlamaServerManager::handleOutput);
    connect(m_process, &QProcess::errorOccurred, this, &LlamaServerManager::handleError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            &LlamaServerManager::handleFinished);
}

LlamaServerManager::~LlamaServerManager()
{
    stopServer();
}

bool LlamaServerManager::startServer(const QString &program, const QString &model,
                                   const QString &host, const QString &port, const QString &temp,
                                   const QString &contextSize, const QString &ngl)
{
    if (m_process->state() == QProcess::NotRunning)
    {
        QStringList arguments;
        arguments << "-m" << model << "-c" << contextSize << "--host" << host << "--port" << port
                 << "-ngl" << ngl << "--temp" << temp;

        QString commandLine = program + " " + arguments.join(" ");
        qDebug() << "启动命令：" << commandLine;

        m_process->start(program, arguments);

        if (m_process->waitForStarted(5000))
        {
            qDebug() << "llama-server服务已启动，进程ID：" << m_process->processId();
            return true;
        }
        else
        {
            qDebug() << "启动llama-server服务失败：" << m_process->errorString();
            return false;
        }
    }
    else
    {
        qDebug() << "llama-server服务已经在运行中";
        return true;
    }
}

void LlamaServerManager::stopServer()
{
    if (m_process->state() != QProcess::NotRunning)
    {
        m_process->terminate();
        if (m_process->waitForFinished(3000))
        {
            qDebug() << "llama-server服务已停止";
        }
        else
        {
            m_process->kill();
            qDebug() << "强制停止llama-server服务";
        }
    }
    else
    {
        qDebug() << "llama-server服务未在运行";
    }
}

void LlamaServerManager::handleOutput()
{
    QString output = m_process->readAllStandardOutput();
    output += m_process->readAllStandardError();
    if (!output.isEmpty())
    {
        qDebug() << "llama-server输出：" << output.trimmed();
    }
}

void LlamaServerManager::handleError(QProcess::ProcessError error)
{
    qDebug() << "llama-server进程错误：" << error;
}

void LlamaServerManager::handleFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "llama-server进程结束，退出代码：" << exitCode << "，退出状态：" << exitStatus;
}
