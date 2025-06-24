TEMPLATE = subdirs
SUBDIRS += Server \
    client \
    tests

QT += core widgets
CONFIG += c++17

# Клиентская часть (GUI)


# Установка версии приложения
VERSION = 0.1
QMAKE_TARGET_BUNDLE = com.example.TeleChannelManager
