TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += link_pkgconfig

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp \
    ControlsWindow.cpp \
    Slider.cpp \
    Window.cpp \
    WindowElement.cpp \
    draw.cpp

PKGCONFIG += opencv

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
    ModBlobDetector.h \
    draw.h
