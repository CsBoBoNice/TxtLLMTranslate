#ifndef OPENAIMANAGER_H
#define OPENAIMANAGER_H

#include <QObject>
#include <QString>
#include <QVector>

class QNetworkAccessManager;
class QNetworkReply;

class OpenaiManager : public QObject
{
    Q_OBJECT
public:
    // 获取单例实例
    static OpenaiManager* getInstance();
    
    // 发送消息到OpenAI API
    QString sendMessage(const QString& url, 
                       const QString& apiKey,
                       const QString& model,
                       const QString& prompt,
                       const QVector<QString>& userPrompts = QVector<QString>(),
                       const QVector<QString>& assistantPrompts = QVector<QString>());
    
    // 测试API连接
    bool testApi(const QString& url, 
                const QString& apiKey,
                const QString& model);

private:
    explicit OpenaiManager(QObject *parent = nullptr);
    ~OpenaiManager();
    
    // 禁用拷贝构造和赋值操作
    OpenaiManager(const OpenaiManager&) = delete;
    OpenaiManager& operator=(const OpenaiManager&) = delete;
    
    // 发送HTTP请求到OpenAI API
    QString sendRequest(const QString& jsonData, 
                       const QString& url, 
                       const QString& apiKey);
    
    static OpenaiManager* instance;
    QNetworkAccessManager* networkManager;
};

#endif // OPENAIMANAGER_H 