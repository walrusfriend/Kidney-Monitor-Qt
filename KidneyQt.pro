QT       += core gui serialport charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Change the build output default directory
CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/build/Debug
} else {
    DESTDIR = $$PWD/build/Release
}

# QXlsx code for Application Qt project
QXLSX_PARENTPATH=./         # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=./header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=./source/  # current QXlsx source path is ./source/
include(./QXlsx.pri)

SOURCES += \
    DataWriter.cpp \
    GraphsTab.cpp \
    ReportUnit.cpp \
    SerialCommunicator.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    DataWriter.h \
    Data_Types.h \
    GraphsTab.h \
    ReportUnit.h \
    SerialCommunicator.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
