#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

Database::Database(QObject *parent) : QObject(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("telechannel.db");
    if (!db.open()) {
        qDebug() << "Failed to open database:" << db.lastError().text();
    } else {
        initializeDatabase();
    }
}

void Database::initializeDatabase()
{
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "username TEXT PRIMARY KEY,"
               "password TEXT NOT NULL)");
    query.exec("CREATE TABLE IF NOT EXISTS tokens ("
               "username TEXT PRIMARY KEY,"
               "token TEXT NOT NULL,"
               "FOREIGN KEY(username) REFERENCES users(username))");
    query.exec("CREATE TABLE IF NOT EXISTS bot_tokens ("
               "username TEXT,"
               "bot_token TEXT NOT NULL,"
               "FOREIGN KEY(username) REFERENCES users(username))");
}

bool Database::addUser(const QString &username, const QString &hashedPassword)
{
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password) VALUES (:username, :password)");
    query.bindValue(":username", username);
    query.bindValue(":password", hashedPassword);
    return query.exec();
}

bool Database::verifyUser(const QString &username, const QString &hashedPassword)
{
    QSqlQuery query;
    query.prepare("SELECT password FROM users WHERE username = :username");
    query.bindValue(":username", username);
    if (query.exec() && query.next()) {
        return query.value(0).toString() == hashedPassword;
    }
    return false;
}

bool Database::removeBotToken(const QString &username, const QString &botToken)
{
    QSqlQuery query;
    query.prepare("DELETE FROM bot_tokens WHERE username = :username AND bot_token = :bot_token");
    query.bindValue(":username", username);
    query.bindValue(":bot_token", botToken);
    return query.exec();
}


void Database::storeToken(const QString &username, const QString &token)
{
    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO tokens (username, token) VALUES (:username, :token)");
    query.bindValue(":username", username);
    query.bindValue(":token", token);
    query.exec();
}

bool Database::addBotToken(const QString &username, const QString &botToken)
{
    QSqlQuery query;
    query.prepare("INSERT INTO bot_tokens (username, bot_token) VALUES (:username, :bot_token)");
    query.bindValue(":username", username);
    query.bindValue(":bot_token", botToken);
    return query.exec();
}



QStringList Database::getBotTokens(const QString &username)
{
    QStringList tokens;
    QSqlQuery query;
    query.prepare("SELECT bot_token FROM bot_tokens WHERE username = :username");
    query.bindValue(":username", username);
    if (query.exec()) {
        while (query.next()) {
            tokens << query.value(0).toString();
        }
    }
    return tokens;
}
