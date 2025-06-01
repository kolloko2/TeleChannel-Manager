#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);
    bool addUser(const QString &username, const QString &hashedPassword);
    bool verifyUser(const QString &username, const QString &hashedPassword);
    void storeToken(const QString &username, const QString &token);
    bool addBotToken(const QString &username, const QString &botToken);
    bool removeBotToken(const QString &username, const QString &botToken);
    QStringList getBotTokens(const QString &username);

private:
    QSqlDatabase db;
    void initializeDatabase();
};

#endif // DATABASE_H
