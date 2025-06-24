#ifndef TEST_MAINWINDOW_H
#define TEST_MAINWINDOW_H

#include <QTest>
#include "../client/mainwindow.h"
#include "../client/clientnetwork.h"

// Мок для сетевого клиента
class MockClientNetwork : public ClientNetwork {
    Q_OBJECT
public:
    MockClientNetwork(QObject *parent = nullptr) : ClientNetwork(parent) {}
    // Перехватываем отправку запроса
    void sendRequest(const QJsonObject &request) override {
        lastRequest = request;
        emit responseReceived(mockResponse);
    }
    QJsonObject lastRequest; // Последний отправленный запрос
    QJsonObject mockResponse; // Мок для ответа
};

// Класс для тестирования MainWindow
class TestMainWindow : public QObject {
    Q_OBJECT
private slots:
    void initTestCase(); // Инициализация перед тестами
    void cleanupTestCase(); // Очистка после тестов
    void testAddTokenClicked(); // Тест добавления токена
    void testSendPostToSelectedChats(); // Тест отправки поста
private:
    MainWindow *window;
    MockClientNetwork *mockNetwork;
};

#endif
