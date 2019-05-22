QT += testlib serialport
QT += gui
CONFIG += qt warn_on depend_includepath testcase

TEMPLATE = app

SOURCES += \    
    tst_testdebughelper.cpp

include(../../BurgerQuiz.pri)
