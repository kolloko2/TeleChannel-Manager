TEMPLATE = subdirs
SUBDIRS += Server \
    client \
    tests

QT += core widgets
CONFIG += c++17

# Клиентская часть (GUI)
SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui



# Установка версии приложения
VERSION = 0.1
QMAKE_TARGET_BUNDLE = com.example.TeleChannelManager
