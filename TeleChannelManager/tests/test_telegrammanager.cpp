#include "test_telegrammanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QSignalSpy>

void TestTelegramManager::initTestCase() {
    // Создаем мок для сетевого менеджера
    mockNetworkManager = new MockNetworkAccessManager(this);
    // Создаем объект TelegramManager
    manager = new TelegramManager(this);
    // Инициализируем с тестовым токеном
    manager->initialize("mock_bot_token");
    // Заменяем реальный сетевой менеджер на мок
    manager->m_networkManager = mockNetworkManager;
}

void TestTelegramManager::cleanupTestCase() {
    // Очищаем ресурсы
    delete manager;
}

void TestTelegramManager::testSendMessage() {
    // Создаем мок для ответа Telegram API
    MockNetworkReply *reply = new MockNetworkReply();
    QJsonObject response;
    response["ok"] = true;
    response["result"] = QJsonObject{{"message_id", 123}, {"chat", QJsonObject{{"id", 456}}}};
    reply->setResponse(QJsonDocument(response).toJson());
    mockNetworkManager->setNextReply(reply);

    // Отслеживаем сигналы ошибок
    QSignalSpy errorSpy(manager, &TelegramManager::errorOccurred);
    // Отправляем тестовое сообщение
    manager->sendMessage(456, "Тестовое сообщение");

    // Проверяем, что ошибок не было
    QVERIFY(errorSpy.count() == 0);
    // Проверяем, что запрос был для отправки сообщения
    QVERIFY(reply->property("requestType").toString() == "sendMessage");
    // Проверяем, что ID чата корректен
    QVERIFY(reply->property("chatId").toLongLong() == 456);
}

void TestTelegramManager::testDeleteCommentsWithFilter() {
    // Создаем мок для ответа getUpdates
    MockNetworkReply *reply = new MockNetworkReply();
    QJsonObject response;
    response["ok"] = true;
    QJsonArray updates;
    QJsonObject msg1;
    msg1["message_id"] = 1;
    msg1["chat"] = QJsonObject{{"id", 123}, {"type", "channel"}};
    msg1["text"] = "Короткое";
    updates.append(QJsonObject{{"message", msg1}});
    QJsonObject msg2;
    msg2["message_id"] = 2;
    msg2["chat"] = QJsonObject{{"id", 123}, {"type", "channel"}};
    msg2["text"] = "Это длинное сообщение для теста";
    updates.append(QJsonObject{{"message", msg2}});
    response["result"] = updates;
    reply->setResponse(QJsonDocument(response).toJson());
    mockNetworkManager->setNextReply(reply);

    // Создаем мок для ответа deleteMessage
    MockNetworkReply *deleteReply = new MockNetworkReply();
    deleteReply->setResponse(QJsonDocument(QJsonObject{{"ok", true}}).toJson());
    mockNetworkManager->setNextReply(deleteReply);

    // Отслеживаем сигналы ошибок
    QSignalSpy errorSpy(manager, &TelegramManager::errorOccurred);
    // Вызываем удаление комментариев с фильтром (мин. 5 слов)
    manager->deleteCommentsWithFilter(5, QStringList());

    // Проверяем, что ошибок не было
    QVERIFY(errorSpy.count() == 0);
    // Проверяем, что запрос на удаление был отправлен
    QVERIFY(deleteReply->property("requestType").toString() == "deleteMessage");
    // Проверяем, что удалено сообщение с ID 1 (короткое)
    QVERIFY(deleteReply->property("messageId").toInt() == 1);
}
