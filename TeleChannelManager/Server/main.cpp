#include "servercontroller.h"
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ServerController server(12345); // Запуск сервера на порту 12345
    return a.exec();
}
