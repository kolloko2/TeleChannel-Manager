#ifndef SERVERCONTROLLER_H
#define SERVERCONTROLLER_H

#include <QTcpServer>
#include <QTcpSocket>
#include "servermodel.h"

class ServerController : public QTcpServer
{
    Q_OBJECT
public:
    explicit ServerController(quint16 port, QObject *parent = nullptr);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void readClientData();
    void clientDisconnected();

private:
    ServerModel *model;
    QMap<qintptr, QTcpSocket*> clients;
};

#endif // SERVERCONTROLLER_H
