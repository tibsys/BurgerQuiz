QT += testlib serialport
QT += gui
CONFIG += qt warn_on depend_includepath testcase

TEMPLATE = app

SOURCES += \
    tst_testmessagefactory.cpp

include(../../BurgerQuiz.pri)
