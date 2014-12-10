TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
unix:CONFIG += link_pkgconfig

QMAKE_CXXFLAGS += -std=c++11

win32:INCLUDEPATH += "%OPENCV_DIR%\include"

unix:PKGCONFIG += opencv libv4l2
win32:LIBS += -L"%OPENCV_LIB_DIR%" \
    -lopencv_core249 \
    -lopencv_highgui249 \
    -lopencv_imgproc249 \
    -lopencv_features2d249

SOURCES += *.cpp
HEADERS += *.h *.hpp

OTHER_FILES += \
    RoboticsCV.pro.user

include(deployment.pri)
qtcAddDeployment()
