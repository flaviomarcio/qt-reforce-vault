QTREFORCE_QVAULT=true
DEFINES+=QTREFORCE_QVAULT

include($$PWD/src/src.pri)
QT+=core network
QT-=gui

INCLUDEPATH += $$PWD/includes

HEADERS+= \
    $$PWD/includes/QtVault
