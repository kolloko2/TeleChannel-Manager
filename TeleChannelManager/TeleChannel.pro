TEMPLATE = subdirs
SUBDIRS += Server \
    client

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



# Настройки для Windows

win32 {
    CONFIG += windows
    # win32:RC_ICONS += app.ico # Раскомментируйте, если есть иконка
    # Замените C:/vcpkg на ваш путь
   
    VCPKG_ROOT = C:/vcpkg
    INCLUDEPATH += $${VCPKG_ROOT}/installed/x64-mingw-dynamic/include
    LIBS += -L$${VCPKG_ROOT}/installed/x64-mingw-dynamic/lib -ltgbot
    LIBS += -lssl -lcrypto -lz -lpthread

}

# Установка версии приложения
VERSION = 0.1
QMAKE_TARGET_BUNDLE = com.example.TeleChannelManager
