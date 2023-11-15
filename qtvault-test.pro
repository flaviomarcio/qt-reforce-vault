QT += testlib

CONFIG += testcase
CONFIG += console
CONFIG += silent
CONFIG -= debug_and_release
QT -= gui

TEMPLATE = app
TARGET = QtVaultTests

INCLUDEPATH+=$$PWD/../src
#INCLUDEPATH+=$$PWD/../src/private

qt_reforce_vault_test_MODE=true
QMAKE_CXXFLAGS += -Dqt_reforce_vault_test_MODE=\\\"$$qt_reforce_vault_test_MODE\\\"

include($$PWD/test/test.pri)


