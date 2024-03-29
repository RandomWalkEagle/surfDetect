TEMPLATE = vcapp
TARGET = testFAST
#DEPENDPATH += ../../opencv/lib
#INCLUDEPATH += .
#INCLUDEPATH += ..
INCLUDEPATH += ../../opencv/include
INCLUDEPATH += ../calTime.h

QMAKE_LIBDIR += "$(TargetDir)"

CONFIG += console
CONFIG += qt warn_on

#win32-msvc* {
QMAKE_CXXFLAGS += /wd4996 /wd4819
#}

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
HEADERS += cvfast.h
HEADERS += drawCorner.h
HEADERS += corner.h
HEADERS += match.h


# source files
SOURCES += cvfast.cpp
SOURCES += corner.cpp
SOURCES += cv_fast_9.cpp
SOURCES += cv_fast_10.cpp
SOURCES += cv_fast_11.cpp
SOURCES += cv_fast_12.cpp
SOURCES += main.cpp
