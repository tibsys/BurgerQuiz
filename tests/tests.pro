QT += testlib serialport
QT += gui
CONFIG += qt warn_on depend_includepath testcase

TEMPLATE = subdirs

SUBDIRS += \
    tst_testserialportcontroller \
    tst_testdebughelper \
    tst_testmessagefactory \

include(../BurgerQuiz.pri)
