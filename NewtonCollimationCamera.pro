QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

LIBS += -L3rdparty/1.17/lib/x64/ -lASICamera2

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += 3rdparty/1.17/include/ asicamera/

SOURCES += \
    src/camera.cpp \
    src/camerainterface.cpp \
    src/cursorgrid.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    \
    asicamera/asicamera.cpp \
    asicamera/asicameracontrol.cpp \
    asicamera/asicamerainfo.cpp \

HEADERS += \
    src/camera.h \
    src/camerainterface.h \
    src/cursorgrid.h \
    src/mainwindow.h \
    \
    asicamera/asicamera.h \
    asicamera/asicamera_p.h \
    asicamera/asicameracontrol.h \
    asicamera/asicamerainfo.h \
    asicamera/asicameracontrol_p.h \
    asicamera/asicamerainfo_p.h

FORMS += \
    src/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
