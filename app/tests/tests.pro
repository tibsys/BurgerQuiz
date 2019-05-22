QT += testlib

CONFIG += qt warn_on depend_includepath testcase

TEMPLATE = subdirs

SUBDIRS += \
    tst_testserialportcontroller \
    tst_testdebughelper \
    tst_testmessagefactory \
    tst_testburgercontroller

include(../BurgerQuiz.pri)
