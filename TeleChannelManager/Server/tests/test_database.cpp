/**
 * @file test_database.cpp
 * @brief Unit tests for Database class of TeleChannel Manager server.
 * @author Nikita Andreev
 * @date 2025-06-22
 */

#include <QtTest/QtTest>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "database.h"

/**
 * @class TestDatabase
 * @brief Test suite for Database class.
 */
class TestDatabase : public QObject {
    Q_OBJECT

private:
    Database *db;

private slots:
    /**
     * @brief Initialize test case.
     */
    void initTestCase() {
        db = new Database(this);
        QVERIFY2(db != nullptr, "Database instance should be created.");
    }

    /**
     * @brief Test database initialization.
     */
    void testDatabaseInitialization() {
        QSqlDatabase sqlDb = QSqlDatabase::database();
        QVERIFY2(sqlDb.isOpen(), "Database should be open.");
        QSqlQuery query;
        QVERIFY2(query.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='users'"),
                 "Users table should exist.");
    }

    /**
     * @brief Test adding a user.
     */
    void testAddUser() {
        QString username = "testuser";
        QString password = "hashedpass123";
        bool success = db->addUser(username, password);
        QVERIFY2(success, "User should be added successfully.");

        QSqlQuery query;
        query.prepare("SELECT password FROM users WHERE username = :username");
        query.bindValue(":username", username);
        QVERIFY2(query.exec() && query.next(), "User should exist in database.");
        QCOMPARE(query.value(0).toString(), password, "Password should match.");
    }

    /**
     * @brief Test verifying a user.
     */
    void testVerifyUser() {
        QString username = "verifyuser";
        QString password = "hashedpass456";
        db->addUser(username, password);
        QVERIFY2(db->verifyUser(username, password), "User verification should succeed.");
        QVERIFY2(!db->verifyUser(username, "wrongpass"), "Verification with wrong password should fail.");
    }

    /**
     * @brief Test adding and retrieving bot tokens.
     */
    void testBotTokens() {
        QString username = "bottokenuser";
        QString botToken = "bot:123456789:ABCDEF";
        db->addUser(username, "hashedpass789");
        bool success = db->addBotToken(username, botToken);
        QVERIFY2(success, "Bot token should be added successfully.");

        QStringList tokens = db->getBotTokens(username);
        QVERIFY2(tokens.contains(botToken), "Bot token should be retrievable.");
    }

    /**
     * @brief Test removing a bot token.
     */
    void testRemoveBotToken() {
        QString username = "removebotuser";
        QString botToken = "bot:987654321:XYZ";
        db->addUser(username, "hashedpass000");
        db->addBotToken(username, botToken);

        bool success = db->removeBotToken(username, botToken);
        QVERIFY2(success, "Bot token should be removed successfully.");

        QStringList tokens = db->getBotTokens(username);
        QVERIFY2(!tokens.contains(botToken), "Removed bot token should not be retrievable.");
    }

    /**
     * @brief Cleanup test case.
     */
    void cleanupTestCase() {
        QSqlQuery query;
        query.exec("DROP TABLE users");
        query.exec("DROP TABLE tokens");
        query.exec("DROP TABLE bot_tokens");
        delete db;
    }
};

QTEST_MAIN(TestDatabase)
#include "test_database.moc"