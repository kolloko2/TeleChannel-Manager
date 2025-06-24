QT += testlib core gui network sql widgets
CONFIG += c++17 console
CONFIG -= app_bundle

# Исходные файлы тестов
SOURCES += \
    test_database.cpp \
    test_mainwindow.cpp \
    test_servermodel.cpp \
    test_telegrammanager.cpp

# Заголовочные файлы тестов
HEADERS += \
    test_database.h \
    test_mainwindow.h \
    test_servermodel.h \
    test_telegrammanager.h

# Путь к исходным файлам клиента и сервера
INCLUDEPATH += ../client ../Server

SOURCES += \
    ../client/mainwindow.cpp \
    ../client/telegrammanager.cpp \
    ../client/clientnetwork.cpp \
    ../client/authwindow.cpp \
    ../Server/database.cpp \
    ../Server/servermodel.cpp \
    ../Server/servercontroller.cpp

HEADERS += \
    ../client/mainwindow.h \
    ../client/telegrammanager.h \
    ../client/clientnetwork.h \
    ../client/authwindow.h \
    ../Server/database.h \
    ../Server/servermodel.h \
    ../Server/servercontroller.h

FORMS += \
    ../client/mainwindow.ui \

