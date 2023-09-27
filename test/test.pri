CONFIG += console
CONFIG -= debug_and_release
CONFIG += testcase
QT += testlib

LC_ALL=en_US.UTF-8

include($$PWD/../qt-reforce-vault.pri)

Q_STM_TEST=true
QMAKE_CXXFLAGS += -DQ_STM_TEST=\\\"$$Q_STM_TEST\\\"

INCLUDEPATH+=$$PWD/src

HEADERS += \
    $$PWD/qt_reforce_vault_test.h \
    $$PWD/qt_reforce_vault_test_main.h \
    $$PWD/qt_reforce_vault_test_kv_client.h \

SOURCES += \
    $$PWD/qt_reforce_vault_test.cpp \
    $$PWD/qt_reforce_vault_test_main.cpp \
