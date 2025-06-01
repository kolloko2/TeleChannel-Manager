#ifndef CLIENTNETWORK_H
#define CLIENTNETWORK_H

#include <QTcpSocket>
#include <QObject>
#include <QJsonObject>

class ClientNetwork : public QObject
{
    Q_OBJECT
public:
    explicit ClientNetwork(QObject *parent = nullptr);
    void connectToServer(const QString &host, quint16 port);
    void sendRequest(const QJsonObject &request);

signals:
    void responseReceived(const QJsonObject &response);
    void connectionError(const QString &error);
    void connected();

private slots:
    void onReadyRead();
    void onErrorOccurred(QAbstractSocket::SocketError error);
    void onConnected();

private:
    QTcpSocket *socket;
    QString host;
    quint16 port;
    void reconnect();
};

#endif
