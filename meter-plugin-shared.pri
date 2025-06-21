# Version=0.0.2
#
QT       += core

linux-beagleboard-g++:{
   QT -= gui
}
INCLUDEPATH  += $$PWD\
                $$PWD/../../defines/defines

HEADERS += \
    $$PWD/shared/addmeterdatahelper.h \
    $$PWD/shared/findemptydate.h \
    $$PWD/shared/meterpluginhelper.h

SOURCES += \
    $$PWD/shared/addmeterdatahelper.cpp \
    $$PWD/shared/findemptydate.cpp \
    $$PWD/shared/meterpluginhelper.cpp

include(../plugin-types/plugin-types.pri)




