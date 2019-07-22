APPNAME = vtkdemo

BUILD = app

#DEFINES += USE_GLEW
DEFINES += USE_OPENGL

LOCAL_DEPS += vtkGUISupportQt vtkglew vtkfreetype vtkzlib
LOCAL_DEPS += vtkViewsQt

include(../../uelib/uelib/uelib.pri)


#-------------------------------------------------------------------#
CONFIG += qt
QT += opengl

include($${UELIBDIR_EXTERNAL}/vtk/vtk.pri)

INCLUDEPATH += $$VTKDIR/GUISupport/Qt $$VTKDIR/GUISupport/QtOpenGL


SOURCES += \
#    main.cpp \
    mainwindow.cpp \
#    Cone6.cxx \
    main2.cxx \
    SimpleView.cxx

HEADERS += \
    mainwindow.h \
    SimpleView.h

FORMS += \
    SimpleView.ui

RESOURCES += \
    Icons/icons.qrc
