TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
unix:CONFIG += link_pkgconfig

QMAKE_CXXFLAGS += -std=c++11

unix:PKGCONFIG += opencv
win32:LIBS += "%OPENCV_LIB_DIR%"

SOURCES += *.cpp
HEADERS += *.h *.hpp

OTHER_FILES += \
    RoboticsCV.pro.user

include(deployment.pri)
qtcAddDeployment()
