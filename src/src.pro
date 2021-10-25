#-------------------------------------------------
#
# Project created by QtCreator 2016-02-09T08:58:44
#
#-------------------------------------------------

QT       -= core gui

TARGET = RTKLib
TEMPLATE = lib
CONFIG += staticlib

include(../RTKLib.pri)
INCLUDEPATH +=

DESTDIR = $${OUT_PWD}

QMAKE_CFLAGS += -std=c99 -pedantic -Wall -Werror -fpic -fno-strict-overflow \
    -Wno-error=unused-but-set-variable -Wno-error=unused-function \
    -Wno-error=unused-result -Wno-error=pointer-to-int-cast \
    -Wno-error=unused-variable -Wno-error=int-conversion \
    -Wno-error=stringop-truncation -Wno-error=format-overflow \
    -Wno-error=maybe-uninitialized -Wno-error=array-bounds
QMAKE_CFLAGS_DEBUG = -O0 -g
QMAKE_CFLAGS_RELEASE = -O3
QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO = -O3 -g
CONFIG -= warn_on

#CONFIG(release, debug|release) {
#    DEFINES += NDEBUG
#}

DEFINES -= UNICODE

SOURCES += rtkcmn.c \
    convkml.c \
    convrnx.c \
    convgpx.c \
    datum.c \
    download.c \
    ephemeris.c \
    geoid.c \
    gis.c \
    ionex.c \
    lambda.c \
    options.c \
    pntpos.c \
    postpos.c \
    ppp.c \
    ppp_ar.c \
    preceph.c \
    rcvraw.c \
    rinex.c \
    rtcm.c \
    rtcm2.c \
    rtcm3.c \
    rtcm3e.c \
    rtkpos.c \
    rtksvr.c \
    sbas.c \
    solution.c \
    stream.c \
    streamsvr.c \
    tides.c \
    tle.c \
    rcv/binex.c \
    rcv/crescent.c \
    rcv/javad.c \
    rcv/novatel.c \
    rcv/nvs.c \
    rcv/rt17.c \
    rcv/septentrio.c \
    rcv/skytraq.c \
    rcv/ss2.c \
    rcv/ublox.c \


HEADERS += rtklib.h \


unix {
    target.path = /usr/lib
    INSTALLS += target
}
