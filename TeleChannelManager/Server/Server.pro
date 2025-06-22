/**
 * @file Server.pro
 * @brief Project file for TeleChannel Manager server with unit tests.
 * @author Anastasia Paramonycheva
 * @date 2025-06-22
 */

QT += core network sql testlib
CONFIG += console c++17
CONFIG -= app_bundle

SOURCES += \
    database.cpp \
    main.cpp \
    servercontroller.cpp \
    servermodel.cpp \
    tests/test_database.cpp \
    tests/test_servermodel.cpp \
    tests/test_servercontroller.cpp

HEADERS += \
    database.h \
    main.h \
    servercontroller.h \
    servermodel.h

# Настройки для Windows
win32 {
    VCPKG_ROOT = C:/vcpkg
    INCLUDEPATH += $${VCPKG_ROOT}/installed/x64-mingw-dynamic/include
    LIBS += -L$${VCPKG_ROOT}/installed/x64-mingw-dynamic/lib -ltgbot
    LIBS += -lssl -lcrypto -lz -lpthread
}
