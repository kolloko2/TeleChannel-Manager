#include "servercontroller.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>

ServerController::ServerController(quint16 port, QObject *parent)
    : QTcpServer(parent), model(new ServerModel(this))
{
    if (!listen(QHostAddress::Any, port)) {
        qDebug() << "Server could not start!";
    } else {
        qDebug() << "Server started on port" << port;
    }
}

void ServerController::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *client = new QTcpSocket(this);
    client->setSocketDescriptor(socketDescriptor);
    clients[socketDescriptor] = client;

    connect(client, &QTcpSocket::readyRead, this, &ServerController::readClientData);
    connect(client, &QTcpSocket::disconnected, this, &ServerController::clientDisconnected);

    qDebug() << "New client connected:" << socketDescriptor;
}

void ServerController::readClientData()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    QByteArray data = client->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();

    QString action = obj["action"].toString();
    QJsonObject response;

    if (action == "register") {
        QString username = obj["username"].toString();
        QString password = obj["password"].toString();
        bool success = model->registerUser(username, password);
        response["action"] = "register_response";
        response["success"] = success;
        response["message"] = success ? "Registration successful" : "Username already exists";
    } else if (action == "login") {
        QString username = obj["username"].toString();
        QString password = obj["password"].toString();
        QString token = model->loginUser(username, password);
        response["action"] = "login_response";
        response["success"] = !token.isEmpty();
        response["token"] = token;
        response["message"] = token.isEmpty() ? "Invalid credentials" : "Login successful";
    } else if (action == "add_bot_token") {
        QString username = obj["username"].toString();
        QString botToken = obj["bot_token"].toString();
        bool success = model->addBotToken(username, botToken);
        response["action"] = "add_bot_token_response";
        response["success"] = success;
        response["message"] = success ? "Bot token added" : "Failed to add bot token";
    } else if (action == "get_bot_tokens") {
        QString username = obj["username"].toString();
        QStringList tokens = model->getBotTokens(username);
        QJsonArray tokenArray;
        for (const QString &token : tokens) {
            tokenArray.append(token);
        }
        response["action"] = "get_bot_tokens_response";
        response["success"] = !tokens.isEmpty();
        response["tokens"] = tokenArray;
        response["message"] = tokens.isEmpty() ? "No bot tokens found" : "Bot tokens retrieved";
    }
    else if (action == "remove_bot_token") {
        QString username = obj["username"].toString();
        QString botToken = obj["bot_token"].toString();
        bool success = model->removeBotToken(username, botToken);
        response["action"] = "remove_bot_token_response";
        response["success"] = success;
        response["message"] = success ? "Bot token removed" : "Failed to remove bot token";
    }



    client->write(QJsonDocument(response).toJson());
}

void ServerController::clientDisconnected()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    qintptr descriptor = client->socketDescriptor();
    clients.remove(descriptor);
    client->deleteLater();
    qDebug() << "Client disconnected:" << descriptor;
}
