TEMPLATE = vcapp
TARGET = testEagleMatch
#DEPENDPATH += ../../opencv/lib
#INCLUDEPATH += .
#INCLUDEPATH += ..
INCLUDEPATH += ../../opencv/include
INCLUDEPATH += ../calTime.h

QMAKE_LIBDIR += "$(TargetDir)"

CONFIG += console
DESTDIR=../../Bin/debug 

CONFIG(debug, debug|release) {
	DESTDIR = ../../Bin/debug 
    QMAKE_LIBDIR += ../Bin/debug
}

CONFIG(release, debug|release) {
	DESTDIR=../../Bin/release
    QMAKE_LIBDIR += ../Bin/release	
}

LIBS += ../../opencv/lib/cv210.lib
LIBS += ../../opencv/lib/cv210d.lib
LIBS += ../../opencv/lib/cvaux210.lib
LIBS += ../../opencv/lib/cvaux210d.lib
LIBS += ../../opencv/lib/cxcore210.lib
LIBS += ../../opencv/lib/cxcore210d.lib
LIBS += ../../opencv/lib/highgui210.lib
LIBS += ../../opencv/lib/highgui210d.lib

# header files
HEADERS += utils.h
HEADERS += match.h

# source files
SOURCES += main.cpp
SOURCES += utils.cpp
SOURCES += match.cpp