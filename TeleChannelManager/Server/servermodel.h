#ifndef SERVERMODEL_H
#define SERVERMODEL_H

#include <QObject>
#include "database.h"

class ServerModel : public QObject
{
    Q_OBJECT
public:
    explicit ServerModel(QObject *parent = nullptr);
    bool registerUser(const QString &username, const QString &password);
    QString loginUser(const QString &username, const QString &password);
    bool addBotToken(const QString &username, const QString &botToken);
    QStringList getBotTokens(const QString &username);
    bool removeBotToken(const QString &username, const QString &botToken);
private:
    Database *db;
    QString generateToken();
};

#endif // SERVERMODEL_H
