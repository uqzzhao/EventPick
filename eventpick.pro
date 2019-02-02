#-------------------------------------------------
#
# Project created by QtCreator 2015-08-24T18:41:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EventPick
TEMPLATE = app


SOURCES += main.cpp\
    util.cpp \
    zoomer.cpp \
    csegy.cpp \
    mer.cpp \


HEADERS  += \
    util.h \
    Zoomer.h \
    csegy.h \
    mer.h \


FORMS    += \
    mer.ui


LIBS+=-L "C:\Qt\Qt5.10.1\5.10.1\mingw53_32\lib"-lqwt
INCLUDEPATH+=C:\Qt\Qt5.10.1\5.10.1\mingw53_32\include\QWT
include(C:\qwt-6.1.3\qwt.prf)

RESOURCES += \
    resources.qrc

RC_FILE = eventpick.rc

