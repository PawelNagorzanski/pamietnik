QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    W_PREZENTACJI/DodajWpis.cpp \
    W_PREZENTACJI/Opcje.cpp \
    W_PREZENTACJI/SzukajWpis.cpp \
    W_DANYCH/Repozytorium.cpp \
    W_DANYCH/Wpis.cpp \
    W_PREZENTACJI/DodajWpis.cpp \
    W_PREZENTACJI/Opcje.cpp \
    W_PREZENTACJI/SzukajWpis.cpp \
    W_PREZENTACJI/mainwindow.cpp \
    W_USLUG/Program.cpp \
    main.cpp \
    W_PREZENTACJI/mainwindow.cpp \

HEADERS += \
    W_PREZENTACJI/DodajWpis.h \
    W_PREZENTACJI/Opcje.h \
    W_PREZENTACJI/SzukajWpis.h \
    W_DANYCH/Repozytorium.h \
    W_DANYCH/Wpis.h \
    W_PREZENTACJI/DodajWpis.h \
    W_PREZENTACJI/Opcje.h \
    W_PREZENTACJI/SzukajWpis.h \
    W_PREZENTACJI/mainwindow.h \
    W_USLUG/Program.h \
    W_PREZENTACJI/mainwindow.h \

FORMS += \
    W_PREZENTACJI/DodajWpis.ui \
    W_PREZENTACJI/Opcje.ui \
    W_PREZENTACJI/SzukajWpis.ui \
    W_PREZENTACJI/DodajWpis.ui \
    W_PREZENTACJI/Opcje.ui \
    W_PREZENTACJI/SzukajWpis.ui \
    W_PREZENTACJI/mainwindow.ui \
    W_PREZENTACJI/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
