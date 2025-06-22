/**
 * @file test_telegrammanager.cpp
 * @brief Unit tests for TelegramManager class of TeleChannel Manager client.
 * @author Vladimir Ilyin
 * @date 2025-06-22
 */

#include <QtTest/QtTest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "telegrammanager.h"

/**
 * @class TestTelegramManager
 * @brief Test suite for TelegramManager class.
 */
class TestTelegramManager : public QObject {
    Q_OBJECT

private:
    TelegramManager *manager;

private slots:
    /**
     * @brief Initialize test case.
     */
    void initTestCase() {
        manager = new TelegramManager(this);
        QVERIFY2(manager != nullptr, "TelegramManager instance should be created.");
    }

    /**
     * @brief Test initialization with valid token.
     */
    void testInitialization() {
        manager->initialize("bot:123456789:ABC");
        QVERIFY2(!manager->property("m_botToken").toString().isEmpty(), "Bot token should be set.");
    }

    /**
     * @brief Test getChats with empty token.
     */
    void testGetChatsEmptyToken() {
        QSignalSpy errorSpy(manager, &TelegramManager::errorOccurred);
        manager->initialize("");
        manager->getChats();
        QVERIFY2(errorSpy.count() == 1, "Empty token should trigger error.");
        QList<QVariant> args = errorSpy.takeFirst();
        QCOMPARE(args.at(0).toString(), QString("Токен бота не установлен"), "Error message should match.");
    }

    /**
     * @brief Test sendMessage with empty token.
     */
    void testSendMessageEmptyToken() {
        QSignalSpy errorSpy(manager, &TelegramManager::errorOccurred);
        manager->initialize("");
        manager->sendMessage(12345, "Test message");
        QVERIFY2(errorSpy.count() == 1, "Empty token should trigger error.");
        QList<QVariant> args = errorSpy.takeFirst();
        QCOMPARE(args.at(0).toString(), QString("Токен бота не установлен"), "Error message should match.");
    }

    /**
     * @brief Cleanup test case.
     */
    void cleanupTestCase() {
        delete manager;
    }
};

QTEST_MAIN(TestTelegramManager)
#include "test_telegrammanager.moc"