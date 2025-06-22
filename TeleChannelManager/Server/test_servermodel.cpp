/**
 * @file test_servermodel.cpp
 * @brief Unit tests for ServerModel class of TeleChannel Manager server.
 * @author Nikita Andreev
 * @date 2025-06-22
 */

#include <QtTest/QtTest>
#include "servermodel.h"

/**
 * @class TestServerModel
 * @brief Test suite for ServerModel class.
 */
class TestServerModel : public QObject {
    Q_OBJECT

private:
    ServerModel *model;

private slots:
    /**
     * @brief Initialize test case.
     */
    void initTestCase() {
        model = new ServerModel(this);
        QVERIFY2(model != nullptr, "ServerModel instance should be created.");
    }

    /**
     * @brief Test user registration.
     */
    void testRegisterUser() {
        QString username = "modeluser";
        QString password = "hashedpass111";
        bool success = model->registerUser(username, password);
        QVERIFY2(success, "User registration should succeed.");
        QVERIFY2(!model->registerUser(username, password), "Duplicate registration should fail.");
    }

    /**
     * @brief Test user login.
     */
    void testLoginUser() {
        QString username = "loginuser";
        QString password = "hashedpass222";
        model->registerUser(username, password);
        QString token = model->loginUser(username, password);
        QVERIFY2(!token.isEmpty(), "Login should return a valid token.");
        QVERIFY2(model->loginUser(username, "wrongpass").isEmpty(), "Login with wrong password should fail.");
    }

    /**
     * @brief Test bot token management.
     */
    void testBotTokenManagement() {
        QString username = "botmodeluser";
        QString botToken = "bot:111222333:ABC";
        model->registerUser(username, "hashedpass333");
        bool success = model->addBotToken(username, botToken);
        QVERIFY2(success, "Bot token should be added successfully.");

        QStringList tokens = model->getBotTokens(username);
        QVERIFY2(tokens.contains(botToken), "Bot token should be retrievable.");

        success = model->removeBotToken(username, botToken);
        QVERIFY2(success, "Bot token should be removed successfully.");
        tokens = model->getBotTokens(username);
        QVERIFY2(!tokens.contains(botToken), "Removed bot token should not be retrievable.");
    }

    /**
     * @brief Test token generation.
     */
    void testGenerateToken() {
        QString token1 = model->generateToken();
        QString token2 = model->generateToken();
        QVERIFY2(!token1.isEmpty(), "Generated token should not be empty.");
        QVERIFY2(token1 != token2, "Generated tokens should be unique.");
    }

    /**
     * @brief Cleanup test case.
     */
    void cleanupTestCase() {
        delete model;
    }
};

QTEST_MAIN(TestServerModel)
#include "test_servermodel.moc"