QT       += core

linux-beagleboard-g++:{
   QT -= gui
}
INCLUDEPATH  += $$PWD\
                $$PWD/../../defines/defines

HEADERS += \
    $$PWD/shared/addmeterdatahelper.h \
    $$PWD/shared/findemptydate.h \
    $$PWD/shared/meterplugin.h \
    $$PWD/shared/meterpluginhelper.h \
    $$PWD/shared/meterplugintypes.h

SOURCES += \
    $$PWD/shared/addmeterdatahelper.cpp \
    $$PWD/shared/findemptydate.cpp \
    $$PWD/shared/meterpluginhelper.cpp





