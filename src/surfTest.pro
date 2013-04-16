TEMPLATE = vcapp
#TARGET = 
#DEPENDPATH += ../opencv/lib
#INCLUDEPATH += .
#INCLUDEPATH += ..
INCLUDEPATH += ../opencv/include
INCLUDEPATH += calTime.h

CONFIG += console

LIBS += ../opencv/lib/cv210.lib
LIBS += ../opencv/lib/cv210d.lib
LIBS += ../opencv/lib/cvaux210.lib
LIBS += ../opencv/lib/cvaux210d.lib
LIBS += ../opencv/lib/cxcore210.lib
LIBS += ../opencv/lib/cxcore210d.lib
LIBS += ../opencv/lib/highgui210.lib
LIBS += ../opencv/lib/highgui210d.lib

# header files
HEADERS += kmeans.h
HEADERS += fasthessian.h
HEADERS += responselayer.h
HEADERS += integral.h
HEADERS += ipoint.h
HEADERS += surf.h
HEADERS += surflib.h
HEADERS += utils.h

# source files
SOURCES += utils.cpp
SOURCES += fasthessian.cpp
SOURCES += main.cpp
SOURCES += integral.cpp
SOURCES += surf.cpp
SOURCES += ipoint.cpp
