#include "servermodel.h"
#include <QUuid>

ServerModel::ServerModel(QObject *parent) : QObject(parent)
{
    db = new Database(this);
}

bool ServerModel::registerUser(const QString &username, const QString &password)
{
    return db->addUser(username, password);
}

QString ServerModel::loginUser(const QString &username, const QString &password)
{
    if (db->verifyUser(username, password)) {
        QString token = generateToken();
        db->storeToken(username, token);
        return token;
    }
    return QString();
}

bool ServerModel::addBotToken(const QString &username, const QString &botToken)
{
    return db->addBotToken(username, botToken);
}

QStringList ServerModel::getBotTokens(const QString &username)
{
    return db->getBotTokens(username);
}

QString ServerModel::generateToken()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

  bool removeBotToken(const QString &username, const QString &botToken);

bool ServerModel::removeBotToken(const QString &username, const QString &botToken)
{
    return db->removeBotToken(username, botToken);
}
