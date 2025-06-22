

QT += core network sql testlib
CONFIG += console c++17
CONFIG -= app_bundle

SOURCES += \
    database.cpp \
    main.cpp \
    servercontroller.cpp \
    servermodel.cpp \
    test_database.cpp \
    test_servermodel.cpp \
    test_servercontroller.cpp

HEADERS += \
    database.h \
    main.h \
    servercontroller.h \
    servermodel.h

