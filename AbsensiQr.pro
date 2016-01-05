#-------------------------------------------------
#
# Project created by QtCreator 2016-01-05T14:23:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AbsensiQr
TEMPLATE = app


SOURCES += main.cpp\
        presensi.cpp \
    use_cvsnakeimage.cpp

HEADERS  += presensi.h \
    use_cvsnakeimage.h

FORMS    += presensi.ui

INCLUDEPATH += /usr/include/opencv
INCLUDEPATH += /usr/include/opencv2
LIBS += -L/usr/lib64 -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_legacy -lzbar -lzint
