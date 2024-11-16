#include "OpenaiManager.h"
#include <QDebug>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

OpenaiManager *OpenaiManager::instance = nullptr;

OpenaiManager *OpenaiManager::getInstance()
{
    if (instance == nullptr)
    {
        instance = new OpenaiManager();
    }
    return instance;
}

OpenaiManager::OpenaiManager(QObject *parent)
    : QObject(parent), networkManager(new QNetworkAccessManager(this))
{
    qDebug() << "初始化OpenAI管理器";
}

OpenaiManager::~OpenaiManager()
{
    delete networkManager;
}

QString OpenaiManager::sendMessage(const QString &url, const QString &apiKey, const QString &model,
                                   const QString &prompt, const QVector<QString> &userPrompts,
                                   const QVector<QString> &assistantPrompts)
{
    QJsonArray messages;

    // 添加系统消息
    QJsonObject systemMessage;
    systemMessage["role"]    = "system";
    systemMessage["content"] = prompt;
    messages.append(systemMessage);

    // 添加示例对话
    int maxExamples = qMin(userPrompts.size(), assistantPrompts.size());
    for (int i = 0; i < maxExamples; ++i)
    {
        // 添加用户示例
        QJsonObject userMessage;
        userMessage["role"]    = "user";
        userMessage["content"] = userPrompts[i];
        messages.append(userMessage);

        // 添加助手示例
        QJsonObject assistantMessage;
        assistantMessage["role"]    = "assistant";
        assistantMessage["content"] = assistantPrompts[i];
        messages.append(assistantMessage);
    }

    // 如果还有剩余的用户示例，继续添加
    for (int i = maxExamples; i < userPrompts.size(); ++i)
    {
        QJsonObject userMessage;
        userMessage["role"]    = "user";
        userMessage["content"] = userPrompts[i];
        messages.append(userMessage);
    }

    // 如果还有剩余的助手示例，继续添加
    for (int i = maxExamples; i < assistantPrompts.size(); ++i)
    {
        QJsonObject assistantMessage;
        assistantMessage["role"]    = "assistant";
        assistantMessage["content"] = assistantPrompts[i];
        messages.append(assistantMessage);
    }

    // 构建请求JSON
    QJsonObject jsonObject;
    jsonObject["model"]    = model;
    jsonObject["messages"] = messages;

    QJsonDocument jsonDoc(jsonObject);
    QString jsonString = jsonDoc.toJson();
    // qDebug() << "\n完整请求JSON:";
    // qDebug().noquote() << jsonString;

    return sendRequest(jsonString, url, apiKey);
}

bool OpenaiManager::testApi(const QString &url, const QString &apiKey, const QString &model)
{
    try
    {
        // 使用默认的测试提示
        QString testPrompt           = "You are a useful assistant!";
        QVector<QString> userPrompts = {"hi", "How are you?"};
        QVector<QString> assistantPrompts = {"Hello!"};

        QString response = sendMessage(url, apiKey, model, testPrompt, userPrompts, assistantPrompts);
        qDebug() << "测试响应:" << response;
        return !response.isEmpty();
    }
    catch (...)
    {
        qDebug() << "API测试失败";
        return false;
    }
}

QString OpenaiManager::sendRequest(const QString &jsonData, const QString &url,
                                   const QString &apiKey)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(apiKey).toUtf8());

    QNetworkReply *reply = networkManager->post(request, jsonData.toUtf8());

    // 使用事件循环等待响应
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "网络错误:" << reply->errorString();
        qDebug() << "错误代码:" << reply->error();
        qDebug() << "HTTP状态码:"
                 << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        reply->deleteLater();
        return QString();
    }

    // 解析响应
    QByteArray responseData = reply->readAll();
    // qDebug() << "原始响应数据:";
    // qDebug().noquote() << QString::fromUtf8(responseData);

    QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObject     = jsonResponse.object();

    QString result;
    if (jsonObject.contains("choices") && jsonObject["choices"].isArray())
    {
        QJsonArray choices = jsonObject["choices"].toArray();
        if (!choices.isEmpty())
        {
            QJsonObject choice = choices[0].toObject();
            if (choice.contains("message") && choice["message"].isObject())
            {
                QJsonObject message = choice["message"].toObject();
                result              = message["content"].toString();
            }
        }
    }

    if (result.isEmpty())
    {
        qDebug() << "警告: 未能从响应中提取有效内容";
    }

    reply->deleteLater();
    return result;
}