#include "clientnetwork.h"
#include <QTimer>
#include <QDebug>
#include <QJsonDocument>

ClientNetwork::ClientNetwork(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &ClientNetwork::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, &ClientNetwork::onErrorOccurred);
    connect(socket, &QTcpSocket::connected, this, &ClientNetwork::onConnected);
}

void ClientNetwork::connectToServer(const QString &host, quint16 port)
{
    this->host = host;
    this->port = port;
    socket->connectToHost(host, port);
}

void ClientNetwork::reconnect()
{
    QTimer::singleShot(3000, this, [this]() {
        qDebug() << "Attempting to reconnect...";
        socket->connectToHost(host, port);
    });
}

void ClientNetwork::onConnected()
{
    emit connected();
}

void ClientNetwork::sendRequest(const QJsonObject &request)
{
    if(socket->state() == QTcpSocket::ConnectedState) {
        QJsonDocument doc(request);
        socket->write(doc.toJson());
    }
}

void ClientNetwork::onReadyRead()
{
    QByteArray data = socket->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if(!doc.isNull() && doc.isObject()) {
        QJsonObject jsonObj = doc.object();
        emit responseReceived(jsonObj);
    }
}

void ClientNetwork::onErrorOccurred(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error)
    const QString errorMsg = socket->errorString();
    emit connectionError(errorMsg);

    if(error != QAbstractSocket::RemoteHostClosedError) {
        reconnect();
    }
}
