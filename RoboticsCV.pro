TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp \
    ControlsWindow.cpp \
    Slider.cpp \
    Window.cpp \
    WindowElement.cpp

INCLUDEPATH += /usr/include
LIBS += pkg-config --libs opencv

include(deployment.pri)
qtcAddDeployment()

OTHER_FILES += \
    RoboticsCV.pro.user

HEADERS += \
    ControlsWindow.h \
    rapidxml.hpp \
    rapidxml_iterators.hpp \
    rapidxml_print.hpp \
    rapidxml_utils.hpp \
    Slider.h \
    targetver.h \
    VisionTest.h \
    Window.h \
    WindowElement.h \
    ModBlobDetector.h
