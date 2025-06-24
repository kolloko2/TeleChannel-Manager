#include "test_servermodel.h"

void TestServerModel::initTestCase() {
    // Создаем объект ServerModel
    model = new ServerModel(this);
    // Используем базу в памяти
    model->db->db = QSqlDatabase::addDatabase("QSQLITE", "test_db");
    model->db->db.setDatabaseName(":memory:");
    QVERIFY(model->db->db.open());
    // Инициализируем структуру базы
    model->db->initializeDatabase();
}

void TestServerModel::cleanupTestCase() {
    // Закрываем и удаляем базу
    model->db->db.close();
    QSqlDatabase::removeDatabase("test_db");
    delete model;
}

void TestServerModel::testAddBotToken() {
    // Добавляем тестовый токен
    bool result = model->addBotToken("test_user", "bot_token_123");
    // Проверяем, что добавление успешно
    QVERIFY(result);

    // Проверяем, что токен сохранен
    QStringList tokens = model->getBotTokens("test_user");
    QVERIFY(tokens.contains("bot_token_123"));
    QVERIFY(tokens.size() == 1);
}
