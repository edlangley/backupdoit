#-------------------------------------------------
#
# Backupdoit project file
#
#-------------------------------------------------
QT += network

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

symbian:TARGET.UID3 = 0xECD8A708

# Smart Installer package's UID
# This UID is from the protected range and therefore the package will
# fail to install if self-signed. By default qmake uses the unprotected
# range value if unprotected UID is defined for the application and
# 0x2002CCCF value if protected UID is given to the application
#symbian:DEPLOYMENT.installer_header = 0x2002CCCF

# Allow network access on Symbian
symbian:TARGET.CAPABILITY += NetworkServices

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

# Speed up launching on MeeGo/Harmattan when using applauncherd daemon
# CONFIG += qdeclarative-boostable

# Add dependency to Symbian components
# CONFIG += qt-components

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    bdlogic.cpp

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

HEADERS += \
    bdlogic.h

RESOURCES += ../backupdoit_qml/qml_resources.qrc
DEFINES += QMLBUILTIN

win32 {
    CONFIG += link_prl

    DEFINES += QJSON_MAKESTATIC
    INCLUDEPATH += ./qjson_win32_mingw4.8/include
    LIBS += -L$$PWD/qjson_win32_mingw4.8/lib -lqjson
}
unix {
    LIBS += -lqjson
}
