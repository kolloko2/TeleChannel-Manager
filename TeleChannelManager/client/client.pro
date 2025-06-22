

QT += core gui network widgets testlib
CONFIG += c++17


SOURCES += \
    main.cpp \
    mainwindow.cpp \
    authwindow.cpp \
    clientnetwork.cpp \
    telegrammanager.cpp \
    test_authwindow.cpp \
    test_mainwindow.cpp \
    test_clientnetwork.cpp \
    test_telegrammanager.cpp

HEADERS += \
    mainwindow.h \
    authwindow.h \
    clientnetwork.h \
    telegrammanager.h

FORMS += \
    mainwindow.ui
