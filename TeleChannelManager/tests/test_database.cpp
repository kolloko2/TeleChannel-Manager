#include "test_database.h"

void TestDatabase::initTestCase() {
    // Создаем объект базы данных
    db = new Database(this);
    // Используем базу в памяти для тестов
    db->db = QSqlDatabase::addDatabase("QSQLITE", "test_db");
    db->db.setDatabaseName(":memory:");
    QVERIFY(db->db.open());
    // Инициализируем структуру базы
    db->initializeDatabase();
}

void TestDatabase::cleanupTestCase() {
    // Закрываем и удаляем базу
    db->db.close();
    QSqlDatabase::removeDatabase("test_db");
    delete db;
}

void TestDatabase::testAddBotToken() {
    // Добавляем тестовый токен
    bool result = db->addBotToken("test_user", "bot_token_123");
    // Проверяем, что добавление успешно
    QVERIFY(result);

    // Проверяем, что токен сохранен в базе
    QStringList tokens = db->getBotTokens("test_user");
    QVERIFY(tokens.contains("bot_token_123"));
    QVERIFY(tokens.size() == 1);
}
