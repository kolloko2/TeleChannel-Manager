#include "telegrammanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QUrlQuery>
#include <QTimer>

TelegramManager::TelegramManager(QObject *parent)
    : QObject(parent), m_networkManager(new QNetworkAccessManager(this)), m_pendingChatChecks(0)
{
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &TelegramManager::onNetworkReplyFinished);
}

TelegramManager::~TelegramManager()
{
}

void TelegramManager::initialize(const QString &botToken)
{
    m_botToken = botToken;
    qDebug() << "TelegramManager initialized with token:" << m_botToken;
}

void TelegramManager::getChats()
{
    if (m_botToken.isEmpty()) {
        emit errorOccurred("Токен бота не установлен");
        return;
    }

    QString url = QString("https://api.telegram.org/bot%1/getUpdates").arg(m_botToken);
    QNetworkRequest request{QUrl(url)};
    QNetworkReply *reply = m_networkManager->get(request);
    reply->setProperty("requestType", "getUpdates");
    qDebug() << "Отправлен запрос getUpdates для получения списка чатов";
}

void TelegramManager::sendMessage(qint64 chatId, const QString &text)
{
    if (m_botToken.isEmpty()) {
        emit errorOccurred("Токен бота не установлен");
        return;
    }

    QUrl url(QString("https://api.telegram.org/bot%1/sendMessage").arg(m_botToken));
    QUrlQuery query;
    query.addQueryItem("chat_id", QString::number(chatId));
    query.addQueryItem("text", text);

    QNetworkRequest request{QUrl(url)};
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply *reply = m_networkManager->post(request, query.toString(QUrl::FullyEncoded).toUtf8());
    reply->setProperty("requestType", "sendMessage");
    reply->setProperty("chatId", chatId);
    qDebug() << "Отправлен запрос sendMessage в чат" << chatId << "с текстом:" << text;
}

void TelegramManager::deleteCommentsWithFilter(int minWords, const QStringList &bannedWords)
{
    if (m_botToken.isEmpty()) {
        emit errorOccurred("Токен бота не установлен");
        return;
    }

    QString urlStr = QString("https://api.telegram.org/bot%1/getUpdates").arg(m_botToken);
    QUrl url(urlStr);
    QNetworkRequest request{QUrl(url)};

    QNetworkReply *reply = m_networkManager->get(request);
    reply->setProperty("requestType", "getUpdatesForDelete");
    reply->setProperty("minWords", minWords);
    reply->setProperty("bannedWords", QVariant(bannedWords));
    qDebug() << "Отправлен запрос getUpdates для удаления комментариев";
}

void TelegramManager::onNetworkReplyFinished(QNetworkReply *reply)
{
    QString requestType = reply->property("requestType").toString();
    QByteArray response = reply->readAll();
    QNetworkReply::NetworkError error = reply->error();
    QString errorString = reply->errorString();
    reply->deleteLater();

    if (error != QNetworkReply::NoError) {
        QJsonDocument errorDoc = QJsonDocument::fromJson(response);
        QString apiError;
        if (!errorDoc.isNull() && errorDoc.isObject()) {
            QJsonObject obj = errorDoc.object();
            apiError = obj.value("description").toString();
            if (apiError.isEmpty()) {
                apiError = QString("Код ошибки: %1").arg(obj.value("error_code").toInt());
            }
        } else {
            apiError = QString("Нет ответа API: ") + QString(response);
        }
        qDebug() << "Ошибка сети в" << requestType << ":" << errorString << "(API error: " << apiError << ")";
        if (requestType == "getChatMember") {
            handleGetChatMemberError(reply->property("chatId").toLongLong(), reply->property("chatName").toString(), apiError, response);
        } else {
            emit errorOccurred("Ошибка сети в " + requestType + ": " + apiError);
        }
        return;
    }

    qDebug() << "Ответ от" << requestType << ":" << response;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        emit errorOccurred("Некорректный JSON в ответе " + requestType + ": " + QString(response));
        return;
    }

    QJsonObject root = jsonDoc.object();
    if (!root.value("ok").toBool()) {
        QString errorMsg = root.value("description").toString();
        qDebug() << "Ошибка Telegram API в" << requestType << ":" << errorMsg;
        if (requestType == "getChatMember") {
            handleGetChatMemberError(reply->property("chatId").toLongLong(), reply->property("chatName").toString(), errorMsg, response);
        } else {
            emit errorOccurred("Ошибка Telegram API в " + requestType + ": " + errorMsg);
        }
        return;
    }

    if (requestType == "getUpdates") {
        handleGetUpdatesResponse(root);
    } else if (requestType == "getChatMember") {
        handleGetChatMemberResponse(root, reply->property("chatId").toLongLong(), reply->property("chatName").toString());
    } else if (requestType == "sendMessage") {
        handleSendMessageResponse(root, reply->property("chatId").toLongLong());
    } else if (requestType == "getUpdatesForDelete") {
        handleDeleteCommentsResponse(root, reply->property("minWords").toInt(), reply->property("bannedWords").toStringList());
    }
}

void TelegramManager::handleGetUpdatesResponse(const QJsonObject &root)
{
    QJsonArray results = root.value("result").toArray();
    QSet<qint64> seenChats;
    m_tempChats.clear();
    m_pendingChatChecks = 0;

    for (const QJsonValue &val : results) {
        QJsonObject update = val.toObject();
        QJsonObject message = update.value("message").toObject();
        QJsonObject channelPost = update.value("channel_post").toObject();
        QJsonObject myChatMember = update.value("my_chat_member").toObject();

        QJsonObject chat;
        if (!message.isEmpty()) {
            chat = message.value("chat").toObject();
        } else if (!channelPost.isEmpty()) {
            chat = channelPost.value("chat").toObject();
        } else if (!myChatMember.isEmpty()) {
            chat = myChatMember.value("chat").toObject();
        } else {
            continue;
        }

        QString chatType = chat.value("type").toString();
        if (chatType != "channel") {
            qDebug() << "Пропущен чат с типом:" << chatType;
            continue;
        }

        qint64 chatId = chat.value("id").toVariant().toLongLong();
        if (seenChats.contains(chatId)) {
            continue;
        }
        seenChats.insert(chatId);

        QString title = chat.value("title").toString();
        QString username = chat.value("username").toString();
        QString displayName = !title.isEmpty() ? title : (!username.isEmpty() ? "@" + username : "Канал ID " + QString::number(chatId));

        checkAdminStatus(chatId, displayName, 1);
        qDebug() << "Запланирована проверка чата:" << displayName << "(ID:" << chatId << ")";
    }

    qDebug() << "Всего запланировано проверок:" << m_pendingChatChecks;
    if (m_pendingChatChecks == 0) {
        emit chatsLoaded(m_tempChats);
        qDebug() << "Нет каналов для проверки, отправлен chatsLoaded с" << m_tempChats.size() << "каналами";
    }
}

void TelegramManager::checkAdminStatus(qint64 chatId, const QString &displayName, int attempt)
{
    const int maxAttempts = 3;
    QUrl url(QString("https://api.telegram.org/bot%1/getChatMember").arg(m_botToken));
    QUrlQuery query;
    query.addQueryItem("chat_id", QString::number(chatId));
    query.addQueryItem("user_id", "7863567289"); // ID бота @BotAdminTest123_bot

    QNetworkRequest request{QUrl(url)};
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply *reply = m_networkManager->post(request, query.toString(QUrl::FullyEncoded).toUtf8());
    reply->setProperty("requestType", "getChatMember");
    reply->setProperty("chatId", chatId);
    reply->setProperty("chatName", displayName);
    reply->setProperty("attempt", attempt);
    reply->setProperty("maxAttempts", maxAttempts);
    m_pendingChatChecks++;
    qDebug() << "Отправлен запрос getChatMember для чата" << chatId << "(попытка" << attempt << "), pending checks:" << m_pendingChatChecks;
}

void TelegramManager::handleGetChatMemberResponse(const QJsonObject &root, qint64 chatId, const QString &displayName)
{
    QJsonObject result = root.value("result").toObject();
    QString status = result.value("status").toString();

    if (status == "administrator") {
        m_tempChats.append(qMakePair(displayName.trimmed(), chatId));
        qDebug() << "Бот является администратором в канале" << displayName << "(ID:" << chatId << ")";
    } else {
        qDebug() << "Бот не является администратором в канале" << displayName << "(ID:" << chatId << ", статус:" << status << ")";
    }

    m_pendingChatChecks--;
    qDebug() << "Завершена проверка для чата" << chatId << ", осталось проверок:" << m_pendingChatChecks;
    if (m_pendingChatChecks == 0) {
        qDebug() << "Все проверки завершены, отправлен chatsLoaded с" << m_tempChats.size() << "каналами:" << m_tempChats;
        emit chatsLoaded(m_tempChats);
    }
}

void TelegramManager::handleGetChatMemberError(qint64 chatId, const QString &displayName, const QString &errorMsg, const QByteArray &response)
{
    qDebug() << "Ошибка getChatMember для чата" << chatId << ": " << errorMsg << ", полный ответ:" << response;
    int attempt = m_pendingChatChecks > 0 ? m_pendingChatChecks : 1;
    int maxAttempts = 3;

    if (attempt < maxAttempts) {
        qDebug() << "Повторная попытка getChatMember для чата" << chatId << "после ошибки:" << errorMsg;
        QTimer::singleShot(1000, this, [=]() {
            checkAdminStatus(chatId, displayName, attempt + 1);
        });
    } else {
        qDebug() << "Бот не добавлен в канал" << displayName << "(ID:" << chatId << ") после" << maxAttempts << "попыток. Ошибка:" << errorMsg;
        m_pendingChatChecks--;
        qDebug() << "Завершена проверка для чата" << chatId << ", осталось проверок:" << m_pendingChatChecks;
        if (m_pendingChatChecks == 0) {
            qDebug() << "Все проверки завершены, отправлен chatsLoaded с" << m_tempChats.size() << "каналами:" << m_tempChats;
            emit chatsLoaded(m_tempChats);
        }
    }
}

void TelegramManager::handleSendMessageResponse(const QJsonObject &root, qint64 chatId)
{
    if (root.value("ok").toBool()) {
        qDebug() << "Сообщение успешно отправлено в чат" << chatId;
    } else {
        emit errorOccurred("Ошибка Telegram API в sendMessage: " + root.value("description").toString());
    }
}

void TelegramManager::handleDeleteCommentsResponse(const QJsonObject &root, int minWords, const QStringList &bannedWords)
{
    QJsonArray updates = root.value("result").toArray();
    qDebug() << "Получено обновлений для очистки:" << updates.size();

    for (const QJsonValue &val : updates) {
        QJsonObject update = val.toObject();
        QJsonObject msg = update.value("message").toObject();
        QJsonObject channelPost = update.value("channel_post").toObject();

        if (msg.isEmpty() && channelPost.isEmpty()) {
            continue;
        }

        QJsonObject targetMsg = msg.isEmpty() ? channelPost : msg;
        QJsonObject chat = targetMsg.value("chat").toObject();

        QString chatType = chat.value("type").toString();
        if (chatType != "channel") {
            continue;
        }

        if (!targetMsg.contains("text")) {
            continue;
        }

        QString text = targetMsg.value("text").toString();
        int wordCount = text.split(" ", Qt::SkipEmptyParts).count();
        QString lower = text.toLower();

        bool banned = false;
        for (const QString &word : bannedWords) {
            if (lower.contains(word.trimmed())) {
                banned = true;
                break;
            }
        }

        if (wordCount < minWords || banned) {
            qint64 chatId = chat.value("id").toVariant().toLongLong();
            int msgId = targetMsg.value("message_id").toInt();

            qDebug() << "Удаление сообщения" << msgId << "в канале" << chatId << "(слов:" << wordCount << ")";

            QUrl delUrl(QString("https://api.telegram.org/bot%1/deleteMessage").arg(m_botToken));
            QUrlQuery delQuery;
            delQuery.addQueryItem("chat_id", QString::number(chatId));
            delQuery.addQueryItem("message_id", QString::number(msgId));

            QNetworkRequest delRequest{QUrl(delUrl)};
            delRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
            QNetworkReply *delReply = m_networkManager->post(delRequest, delQuery.toString(QUrl::FullyEncoded).toUtf8());
            delReply->setProperty("requestType", "deleteMessage");
            delReply->setProperty("chatId", chatId);
            delReply->setProperty("messageId", msgId);
        }
    }
}
