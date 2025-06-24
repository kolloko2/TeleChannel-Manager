QT += core gui network widgets

CONFIG += c++17

# Пути VCPKG — если используете другие зависимости из него, оставьте
VCPKG_ROOT = C:/vcpkg
VCPKG_INSTALLED = $${VCPKG_ROOT}/installed/x64-mingw-static

# Путь к заголовочным файлам (если вдруг используете другие библиотеки оттуда)
INCLUDEPATH += "$${VCPKG_INSTALLED}/include"

win32-g++ {
    # Настройки debug/release
    CONFIG(debug, debug|release) {
        # Путь к debug-библиотекам (если используете что-то из vcpkg всё ещё)
        LIBS += -L"$${VCPKG_INSTALLED}/debug/lib"
    } else {
        LIBS += -L"$${VCPKG_INSTALLED}/lib"
    }

    # Обязательные системные библиотеки
    LIBS += \
        -lws2_32 \
        -lcrypt32 \
        -lbcrypt \
        -luserenv \
        -ladvapi32

    # Компилятор и линкер флаги
    QMAKE_CXXFLAGS += -std=c++17
    QMAKE_LFLAGS += -static
}

# Отладочные сообщения
message("Include path: $${VCPKG_INSTALLED}/include")
message("Library path (debug): $${VCPKG_INSTALLED}/debug/lib")
message("Library path (release): $${VCPKG_INSTALLED}/lib")

# Исходные файлы
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    authwindow.cpp \
    clientnetwork.cpp \
    telegrammanager.cpp

HEADERS += \
    mainwindow.h \
    authwindow.h \
    clientnetwork.h \
    telegrammanager.h

FORMS += \
    mainwindow.ui
