#ifndef TEST_DATABASE_H
#define TEST_DATABASE_H

#include <QTest>
#include "../Server/database.h"

// Класс для тестирования Database
class TestDatabase : public QObject {
    Q_OBJECT
private slots:
    void initTestCase(); // Инициализация перед тестами
    void cleanupTestCase(); // Очистка после тестов
    void testAddBotToken(); // Тест добавления токена
private:
    Database *db;
};

#endif
