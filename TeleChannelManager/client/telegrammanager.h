#ifndef TELEGRAMMANAGER_H
#define TELEGRAMMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QList>
#include <QPair>

class TelegramManager : public QObject
{
    Q_OBJECT

public:
    explicit TelegramManager(QObject *parent = nullptr);
    ~TelegramManager();

    void initialize(const QString &botToken);
    void getChats();
    void sendMessage(qint64 chatId, const QString &text);
    void deleteCommentsWithFilter(int minWords, const QStringList &bannedWords);

signals:
    void chatsLoaded(const QList<QPair<QString, qint64>> &chats);
    void errorOccurred(const QString &error);

private slots:
    void onNetworkReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_networkManager;
    QString m_botToken;
    QList<QPair<QString, qint64>> m_tempChats;
    int m_pendingChatChecks;

    void handleGetUpdatesResponse(const QJsonObject &root);
    void checkAdminStatus(qint64 chatId, const QString &displayName, int attempt);
    void handleGetChatMemberResponse(const QJsonObject &root, qint64 chatId, const QString &displayName);
    void handleGetChatMemberError(qint64 chatId, const QString &displayName, const QString &errorMsg, const QByteArray &response);
    void handleSendMessageResponse(const QJsonObject &root, qint64 chatId);
    void handleDeleteCommentsResponse(const QJsonObject &root, int minWords, const QStringList &bannedWords);

};

#endif
