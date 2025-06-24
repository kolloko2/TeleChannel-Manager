#ifndef TEST_SERVERMODEL_H
#define TEST_SERVERMODEL_H

#include <QTest>
#include "../Server/servermodel.h"

// Класс для тестирования ServerModel
class TestServerModel : public QObject {
    Q_OBJECT
private slots:
    void initTestCase(); // Инициализация перед тестами
    void cleanupTestCase(); // Очистка после тестов
    void testAddBotToken(); // Тест добавления токена
private:
    ServerModel *model;
};

#endif
