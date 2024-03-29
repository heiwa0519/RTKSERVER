QT -= gui
QT += core sql

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(../../RTKLib.pri)

TARGET = server_mult.v1.0
TEMPLATE = app

INCLUDEPATH += ../../src


linux{
    RTKLIB =../../src/libRTKLib.a
    LIBS +=  $${RTKLIB}
}
macx{
    RTKLIB =../../src/libRTKLib.a
    LIBS += /usr/local/lib/libpng.a $${RTKLIB}
}
win32 {
    RTKLIB =../../src/libRTKLib.a
    LIBS+= $${RTKLIB} -lWs2_32 -lwinmm
}

PRE_TARGETDEPS = $${RTKLIB}


SOURCES += \
        main.cpp \
    servermain.cpp


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

HEADERS += \
    server_mult.h \
    svrsettings.h

