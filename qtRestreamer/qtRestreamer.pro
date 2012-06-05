
QT       += core network

QT       -= gui

TARGET = qtRestreamer
CONFIG   += console
CONFIG   -= app_bundle
CONFIG += debug_and_release

TEMPLATE = app


OBJECTS_DIR = ./gen/obj
MOC_DIR = ./gen/moc
RCC_DIR = ./gen/rcc
UI_DIR = ./gen/ui

LIBS+= -L../qHttpServer -lqHttpServer -lLogger






SOURCES += main.cpp \
    requestlistener.cpp \
    streamcontroller.cpp \
    abstractseeder.cpp \
    leecher.cpp \
    blanksquareseeder.cpp \
    streammanager.cpp \
    mjpegstreamseeder.cpp

HEADERS += \
    requestlistener.h \
    streamcontroller.h \
    abstractseeder.h \
    leecher.h \
    blanksquareseeder.h \
    streammanager.h \
    streampoint.h \
    mjpegstreamseeder.h

INCLUDEPATH += $$_PRO_FILE_PWD_/../CuteLogger/include \
               $$_PRO_FILE_PWD_/../qHttpServer

INSTALLS += target

