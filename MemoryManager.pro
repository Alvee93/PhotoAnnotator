QT       += core gui sql webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RC_ICONS = p_album.ico

SOURCES += \
    daylightclass.cpp \
    dbmanager.cpp \
    exif.cpp \
    facerec.cpp \
    gcalendar.cpp \
    gmapclass.cpp \
    licdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    weatherclass.cpp

HEADERS += \
    daylightclass.h \
    dbmanager.h \
    exif.h \
    facerec.h \
    gcalendar.h \
    gmapclass.h \
    licdialog.h \
    mainwindow.h \
    weatherclass.h

FORMS += \
    licdialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
