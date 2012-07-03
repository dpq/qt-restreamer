#QHTTPSERVER_BASE = ..
TEMPLATE = lib

TARGET = qHttpServer
VERSION = 0.2.0

QT += network
QT -= gui

CONFIG += dll debug_and_release





MOC_DIR = ./gen/moc
RCC_DIR = ./gen/rcc
UI_DIR = ./gen/ui


CONFIG(release, debug|release) {
    DESTDIR = ../release
    OBJECTS_DIR = ./gen/release/obj
}else{
    DESTDIR = ../debug
    OBJECTS_DIR = ./gen/debug/obj
}


INCLUDEPATH += ../http-parser

PRIVATE_HEADERS += ../http-parser/http_parser.h qhttpconnection.h

PUBLIC_HEADERS += qhttpserver.h qhttprequest.h qhttpresponse.h

HEADERS = $$PRIVATE_HEADERS $$PUBLIC_HEADERS \
    abstractrequesthandler.h \
    threadpoolwithsignals.h \
    controlthread.h
SOURCES = *.cpp ../http-parser/http_parser.c

#
#OBJECTS_DIR = $$_PRO_FILE_PWD_/build
#MOC_DIR = $$_PRO_FILE_PWD_/build
#DESTDIR = $$QHTTPSERVER_BASE/lib

#target.path = $$QHTTPSERVER_BASE/lib
INSTALLS += target
