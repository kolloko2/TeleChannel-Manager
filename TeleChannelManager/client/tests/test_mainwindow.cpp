/**
 * @file test_mainwindow.cpp
 * @brief Unit tests for MainWindow class of TeleChannel Manager client.
 * @author Vladimir Ilyin
 * @date 2025-06-22
 */

#include <QtTest/QtTest>
#include <QJsonObject>
#include <QListWidget>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clientnetwork.h"

/**
 * @class TestMainWindow
 * @brief Test suite for MainWindow class.
 */
class TestMainWindow : public QObject {
    Q_OBJECT

private:
    MainWindow *mainWindow;
    ClientNetwork *network;

private slots:
    /**
     * @brief Initialize test case.
     */
    void initTestCase() {
        network = new ClientNetwork(this);
        mainWindow = new MainWindow(nullptr);
        mainWindow->setNetwork(network);
        mainWindow->setAuthData("test_token", "testuser");
        QVERIFY2(mainWindow != nullptr, "MainWindow instance should be created.");
    }

    /**
     * @brief Test UI initialization.
     */
    void testUiInitialization() {
        QVERIFY2(mainWindow->ui->listBotTokens != nullptr, "Bot tokens list should exist.");
        QVERIFY2(mainWindow->ui->txtNewToken != nullptr, "New token input field should exist.");
        QVERIFY2(mainWindow->ui->listChats != nullptr, "Chats list should exist.");
        QVERIFY2(mainWindow->ui->textEditPost != nullptr, "Post text edit should exist.");
    }

    /**
     * @brief Test adding bot token with empty input.
     */
    void testAddEmptyToken() {
        QLineEdit *txtNewToken = mainWindow->ui->txtNewToken;
        txtNewToken->setText("");
        mainWindow->addTokenClicked();

        QSignalSpy spy(mainWindow, &MainWindow::showError);
        QVERIFY2(spy.count() == 1, "Empty token should trigger error.");
        QList<QVariant> args = spy.takeFirst();
        QCOMPARE(args.at(0).toString(), QString("Токен не может быть пустым!"), "Error message should match.");
    }

    /**
     * @brief Test bot token list update.
     */
    void testUpdateTokensList() {
        QStringList tokens = {"bot:123456789:ABC", "bot:987654321:XYZ"};
        mainWindow->updateTokensList(tokens);
        QCOMPARE(mainWindow->ui->listBotTokens->count(), 2, "Token list should contain 2 items.");
        QCOMPARE(mainWindow->ui->listBotTokens->item(0)->text(), QString("bot:123456789:ABC"), "First token should match.");
    }

    /**
     * @brief Test sending post with empty text.
     */
    void testSendEmptyPost() {
        mainWindow->ui->textEditPost->setPlainText("");
        mainWindow->sendPostToSelectedChats();

        QSignalSpy spy(mainWindow, &MainWindow::showError);
        QVERIFY2(spy.count() == 1, "Empty post should trigger error.");
        QList<QVariant> args = spy.takeFirst();
        QCOMPARE(args.at(0).toString(), QString("Текст поста не может быть пустым"), "Error message should match.");
    }

    /**
     * @brief Cleanup test case.
     */
    void cleanupTestCase() {
        delete mainWindow;
        delete network;
    }
};

QTEST_MAIN(TestMainWindow)
#include "test_mainwindow.moc"