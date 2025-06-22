/**
 * @file test_servercontroller.cpp
 * @brief Unit tests for ServerController class of TeleChannel Manager server.
 * @author Nikita Andreev
 * @date 2025-06-22
 */

#include <QtTest/QtTest>
#include <QTcpSocket>
#include "servercontroller.h"

/**
 * @class TestServerController
 * @brief Test suite for ServerController class.
 */
class TestServerController : public QObject {
    Q_OBJECT

private:
    ServerController *server;

private slots:
    /**
     * @brief Initialize test case.
     */
    void initTestCase() {
        server = new ServerController(0, this); // Port 0 for dynamic allocation
        QVERIFY2(server->isListening(), "Server should be listening.");
    }

    /**
     * @brief Test client connection.
     */
    void testClientConnection() {
        QTcpSocket client;
        client.connectToHost(QHostAddress::LocalHost, server->serverPort());
        QVERIFY2(client.waitForConnected(1000), "Client should connect to server.");

        QSignalSpy spy(server, &QTcpServer::newConnection);
        QVERIFY2(spy.wait(1000), "Server should emit newConnection signal.");
    }

    /**
     * @brief Test client registration request.
     */
    void testRegisterRequest() {
        QTcpSocket client;
        client.connectToHost(QHostAddress::LocalHost, server->serverPort());
        QVERIFY2(client.waitForConnected(1000), "Client should connect.");

        QJsonObject request;
        request["action"] = "register";
        request["username"] = "testclient";
        request["password"] = "hashedpass444";

        client.write(QJsonDocument(request).toJson());
        QVERIFY2(client.waitForBytesWritten(1000), "Request should be sent.");

        QVERIFY2(client.waitForReadyRead(1000), "Response should be received.");
        QByteArray responseData = client.readAll();
        QJsonDocument responseDoc = QJsonDocument::fromJson(responseData);
        QJsonObject response = responseDoc.object();

        QVERIFY2(response["success"].toBool(), "Registration should succeed.");
        QCOMPARE(response["message"].toString(), QString("Registration successful"), "Response message should match.");
    }

    /**
     * @brief Test client disconnection.
     */
    void testClientDisconnection() {
        QTcpSocket client;
        client.connectToHost(QHostAddress::LocalHost, server->serverPort());
        QVERIFY2(client.waitForConnected(1000), "Client should connect.");

        client.disconnectFromHost();
        QVERIFY2(client.waitForDisconnected(1000), "Client should disconnect.");
    }

    /**
     * @brief Cleanup test case.
     */
    void cleanupTestCase() {
        server->close();
        delete server;
    }
};

QTEST_MAIN(TestServerController)
#include "test_servercontroller.moc"