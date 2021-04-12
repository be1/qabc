include(../common.pri)
TEMPLATE = lib
TARGET = abc2smf
CONFIG -= qt
CONFIG += link_pkgconfig debug staticlib
DEFINES += _FORTIFY_SOURCE=2
QMAKE_CFLAGS += -O2
PKGCONFIG += smf
SOURCES += abc.c abcparse.c abcsmf.c parser.c
HEADERS += abc.h abcparse.h abcsmf.h parser.h
