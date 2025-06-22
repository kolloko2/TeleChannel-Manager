/**
 * @file client.pro
 * @brief Project file for TeleChannel Manager client with unit tests.
 * @author Anastasia Paramonycheva
 * @date 2025-06-22
 */

QT += core gui network widgets testlib
CONFIG += c++17

VCPKG_ROOT = C:/vcpkg
VCPKG_INSTALLED = $${VCPKG_ROOT}/installed/x64-mingw-static

INCLUDEPATH += "$${VCPKG_INSTALLED}/include"

win32-g++ {
    CONFIG(debug, debug|release) {
        LIBS += -L"$${VCPKG_INSTALLED}/debug/lib"
    } else {
        LIBS += -L"$${VCPKG_INSTALLED}/lib"
    }

    LIBS += \
        -lws2_32 \
        -lcrypt32 \
        -lbcrypt \
        -luserenv \
        -ladvapi32

    QMAKE_CXXFLAGS += -std=c++17
    QMAKE_LFLAGS += -static
}

message("Include path: $${VCPKG_INSTALLED}/include")
message("Library path (debug): $${VCPKG_INSTALLED}/debug/lib")
message("Library path (release): $${VCPKG_INSTALLED}/lib")

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    authwindow.cpp \
    clientnetwork.cpp \
    telegrammanager.cpp \
    tests/test_authwindow.cpp \
    tests/test_mainwindow.cpp \
    tests/test_clientnetwork.cpp \
    tests/test_telegrammanager.cpp

HEADERS += \
    mainwindow.h \
    authwindow.h \
    clientnetwork.h \
    telegrammanager.h

FORMS += \
    mainwindow.ui
