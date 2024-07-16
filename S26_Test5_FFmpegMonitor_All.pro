#-------------------------------------------------
#
# Project created by QtCreator 2014-04-30T10:42:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = S26_Test5_FFmpegMonitor_All
TEMPLATE = app


SOURCES += main.cpp\
        frmmain.cpp \
    avffmpegcamerathread.cpp \
    avffmpegnetcamera.cpp \
    dialogsetting.cpp \
    t3ffmpegh2645encoder2.cpp

HEADERS  += frmmain.h \
    avffmpegcamerathread.h \
    avffmpegnetcamera.h \
    dialogsetting.h \
    t3ffmpegh2645encoder2.h

FORMS    += frmmain.ui \
    dialogsetting.ui
INCLUDEPATH += $$PWD/FFmpeg431dev/include

LIBS += $$PWD/FFmpeg431dev/lib/avcodec.lib  \
        $$PWD/FFmpeg431dev/lib/avdevice.lib  \
        $$PWD/FFmpeg431dev/lib/avfilter.lib  \
        $$PWD/FFmpeg431dev/lib/avformat.lib  \
        $$PWD/FFmpeg431dev/lib/avutil.lib  \
        $$PWD/FFmpeg431dev/lib/swresample.lib  \
        $$PWD/FFmpeg431dev/lib/swscale.lib


MOC_DIR=temp/moc
RCC_DIR=temp/rcc
UI_DIR=temp/ui
OBJECTS_DIR=temp/obj
DESTDIR=bin

win32:RC_FILE=main.rc

RESOURCES += \
    res.qrc
