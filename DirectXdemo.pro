QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    WICTextureLoader11.cpp \
    dxwindow.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    WICTextureLoader11.h \
    dxwindow.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

LIBS += -L$$"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.20348.0/um/x64" -ld3d11 -ld3dcompiler -ldxguid -lwindowscodecs -lOle32
INCLUDEPATH += quote(C:/Program Files (x86)/Windows Kits/10/Include/10.0.20348.0/um)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
