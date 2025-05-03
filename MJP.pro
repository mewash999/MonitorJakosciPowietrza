QT += core network gui widgets charts
TARGET = MJP

greaterThan(QT_MAJOR_VERSION, 4):

CONFIG += c++17

SOURCES += \
    apiclient.cpp \
    apiworker.cpp \
    connectionmanager.cpp \
    datamanager.cpp \
    main.cpp \
    mainwindow.cpp \
    measurementhandler.cpp \
    sensorhandler.cpp \
    stationhandler.cpp

HEADERS += \
    apiclient.h \
    apiworker.h \
    connectionmanager.h \
    datamanager.h \
    mainwindow.h \
    measurementhandler.h \
    sensorhandler.h \
    stationhandler.h

FORMS += \
    mainwindow.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
