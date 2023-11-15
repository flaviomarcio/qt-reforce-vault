QTREFORCE_VAULT=true
DEFINES+=QTREFORCE_VAULT

include($$PWD/src/src.pri)
QT+=core network
QT-=gui

INCLUDEPATH += $$PWD/includes

HEADERS+= \
    $$PWD/includes/QtVault
