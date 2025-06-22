/**
 * @file test_clientnetwork.cpp
 * @brief Unit tests for ClientNetwork class of TeleChannel Manager client.
 * @author Vladimir Ilyin
 * @date 2025-06-22
 */

#include <QtTest/QtTest>
#include <QTcpServer>
#include <QJsonObject>
#include "clientnetwork.h"

/**
 * @class TestClientNetwork
 * @brief Test suite for ClientNetwork class.
 */
class TestClientNetwork : public QObject {
    Q_OBJECT

private:
    ClientNetwork *network;
    QTcpServer *mockServer;

private slots:
    /**
     * @brief Initialize test case.
     */
    void initTestCase() {
        network = new ClientNetwork(this);
        mock審査Server = new QTcpServer(this);
        QVERIFY2(mockServer->listen(QHostAddress::LocalHost, 0), "Mock server should start.");
        QVERIFY2(mockServer->isListening(), "Mock server should be listening.");
    }

    /**
     * @brief Test connection to server.
     */
    void testConnectToServer() {
        QSignalSpy connectedSpy(network, &ClientNetwork::connected);
        network->connectToServer("localhost", mockServer->serverPort());
        QVERIFY2(mockServer->waitForNewConnection(1000), "Mock server should receive connection.");
        QVERIFY2(connectedSpy.wait(1000), "Client should emit connected signal.");
    }

    /**
     * @brief Test sending request.
     */
    void testSendRequest() {
        network->connectToServer("localhost", mockServer->serverPort());
        QVERIFY2(mockServer->waitForNewConnection(1000), "Mock server should receive connection.");

        QJsonObject request;
        request["action"] = "test";
        network->sendRequest(request);

        QTcpSocket *client = mockServer->nextPendingConnection();
        QVERIFY2(client->waitForReadyRead(1000), "Server should receive data.");
        QByteArray data = client->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QVERIFY2(doc.isObject(), "Received data should be valid JSON.");
        QCOMPARE(doc.object()["action"].toString(), QString("test"), "Request action should match.");
    }

    /**
     * @brief Test connection error handling.
     */
    void testConnectionError() {
        QSignalSpy errorSpy(network, &ClientNetwork::connectionError);
        network->connectToServer("invalid_host", 12345);
        QVERIFY2(errorSpy.wait(1000), "Connection to invalid host should emit error signal.");
    }

    /**
     * @brief Cleanup test case.
     */
    void cleanupTestCase() {
        mockServer->close();
        delete mockServer;
        delete network;
    }
};

QTEST_MAIN(TestClientNetwork)
#include "test_clientnetwork.moc"