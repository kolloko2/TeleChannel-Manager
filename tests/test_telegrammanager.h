#ifndef TEST_TELEGRAMMANAGER_H
#define TEST_TELEGRAMMANAGER_H

#include <QTest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "../client/telegrammanager.h"

// Мок для сетевого ответа
class MockNetworkReply : public QNetworkReply {
    Q_OBJECT
public:
    MockNetworkReply(QObject *parent = nullptr) : QNetworkReply(parent) {}
    // Устанавливает данные ответа
    void setResponse(const QByteArray &data) { responseData = data; }
    // Эмулирует чтение ответа
    QByteArray readAll() override { return responseData; }
    // Устанавливает ошибку
    void setError(NetworkError err, const QString &errStr) {
        QNetworkReply::setError(err, errStr);
    }
protected:
    qint64 readData(char *, qint64) override { return -1; }
private:
    QByteArray responseData;
};

// Мок для сетевого менеджера
class MockNetworkAccessManager : public QNetworkAccessManager {
    Q_OBJECT
public:
    MockNetworkAccessManager(QObject *parent = nullptr) : QNetworkAccessManager(parent) {}
    // Устанавливает следующий ответ для обработки
    void setNextReply(MockNetworkReply *reply) { nextReply = reply; }
protected:
    // Перехватывает создание запроса
    QNetworkReply *createRequest(Operation op, const QNetworkRequest &req, QIODevice *outgoingData = nullptr) override {
        return nextReply ? nextReply : QNetworkAccessManager::createRequest(op, req, outgoingData);
    }
private:
    MockNetworkReply *nextReply = nullptr;
};

// Класс для тестирования TelegramManager
class TestTelegramManager : public QObject {
    Q_OBJECT
private slots:
    void initTestCase(); // Инициализация перед тестами
    void cleanupTestCase(); // Очистка после тестов
    void testSendMessage(); // Тест отправки сообщения
    void testDeleteCommentsWithFilter(); // Тест удаления комментариев
private:
    TelegramManager *manager;
    MockNetworkAccessManager *mockNetworkManager;
};

#endif
