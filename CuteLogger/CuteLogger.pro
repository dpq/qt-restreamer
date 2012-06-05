QT       -= gui

TARGET = Logger
TEMPLATE = lib

DEFINES += CUTELOGGER_LIBRARY

INCLUDEPATH += ./include

CONFIG += debug_and_release


OBJECTS_DIR = ./gen/obj
MOC_DIR = ./gen/moc
RCC_DIR = ./gen/rcc
UI_DIR = ./gen/ui


SOURCES += src/Logger.cpp \
           src/AbstractAppender.cpp \
           src/AbstractStringAppender.cpp \
           src/ConsoleAppender.cpp \
           src/FileAppender.cpp

HEADERS += include/Logger.h \
           include/CuteLogger_global.h \
           include/AbstractAppender.h \
           include/AbstractStringAppender.h \
           include/ConsoleAppender.h \
           include/FileAppender.h \
           include/CuteLogger_global.h

win32 {
    SOURCES += src/OutputDebugAppender.cpp
    HEADERS += include/OutputDebugAppender.h
}

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE8FB3D8D
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = CuteLogger.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
