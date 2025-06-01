QT += core network sql
CONFIG += console c++17
CONFIG -= app_bundle

SOURCES += \
    database.cpp \
    main.cpp \
    servercontroller.cpp \
    servermodel.cpp

HEADERS += \
    database.h \
    main.h \
    servercontroller.h \
    servermodel.h