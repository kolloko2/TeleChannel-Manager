#include "test_mainwindow.h"
#include <QSignalSpy>
#include <QListWidget>
#include <QTextEdit>
#include <QPushButton>

void TestMainWindow::initTestCase() {
    // Создаем мок для сетевого клиента
    mockNetwork = new MockClientNetwork(this);
    // Создаем объект MainWindow
    window = new MainWindow();
    // Устанавливаем тестовую сеть и данные авторизации
    window->setNetwork(mockNetwork);
    window->setAuthData("mock_token", "test_user");
}

void TestMainWindow::cleanupTestCase() {
    // Очищаем ресурсы
    delete window;
}

void TestMainWindow::testAddTokenClicked() {
    // Находим поле ввода токена и кнопку добавления
    QLineEdit *txtNewToken = window->findChild<QLineEdit*>("txtNewToken");
    QPushButton *btnAddToken = window->findChild<QPushButton*>("btnAddToken");

    // Устанавливаем тестовый токен
    txtNewToken->setText("new_bot_token");
    // Отслеживаем сигналы ответа
    QSignalSpy responseSpy(mockNetwork, &MockClientNetwork::responseReceived);

    // Эмулируем клик по кнопке
    QTest::mouseClick(btnAddToken, Qt::LeftButton);

    // Проверяем, что запрос корректен
    QVERIFY(mockNetwork->lastRequest["action"].toString() == "add_bot_token");
    QVERIFY(mockNetwork->lastRequest["username"].toString() == "test_user");
    QVERIFY(mockNetwork->lastRequest["bot_token"].toString() == "new_bot_token");
    QVERIFY(responseSpy.count() == 1);
}

void TestMainWindow::testSendPostToSelectedChats() {
    // Находим элементы интерфейса
    QListWidget *listChats = window->findChild<QListWidget*>("listChats");
    QTextEdit *textEditPost = window->findChild<QTextEdit*>("textEditPost");
    QPushButton *btnSendPost = window->findChild<QPushButton*>("btnSendPost");

    // Создаем тестовый чат
    QListWidgetItem *item = new QListWidgetItem("Тестовый чат");
    item->setData(Qt::UserRole, QVariant(qint64(123)));
    listChats->addItem(item);
    listChats->setCurrentItem(item);

    // Создаем тестовый токен
    QListWidget *listBotTokens = window->findChild<QListWidget*>("listBotTokens");
    QListWidgetItem *tokenItem = new QListWidgetItem("mock_bot_token");
    listBotTokens->addItem(tokenItem);
    listBotTokens->setCurrentItem(tokenItem);

    // Создаем мок для TelegramManager
    TelegramManager *mockManager = new TelegramManager(window);
    window->telegramManagers.insert("mock_bot_token", mockManager);

    // Отслеживаем сигналы ошибок
    QSignalSpy messageSpy(mockManager, &TelegramManager::errorOccurred);
    // Устанавливаем текст поста
    textEditPost->setPlainText("Тестовый пост");

    // Эмулируем клик по кнопке отправки
    QTest::mouseClick(btnSendPost, Qt::LeftButton);

    // Проверяем, что ошибок не было
    QVERIFY(messageSpy.count() == 0);
}
