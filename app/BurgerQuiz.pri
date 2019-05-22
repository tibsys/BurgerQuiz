QT += quick serialport multimedia multimediawidgets widgets gui
CONFIG += c++11

coverage {
    QMAKE_CXXFLAGS += --coverage
    QMAKE_LFLAGS += --coverage
}

SOURCES += \
    $$PWD/controllers/SerialPortController.cpp \
    $$PWD/controllers/GameController.cpp \
    $$PWD/domain/Burger.cpp \
    $$PWD/controllers/BurgerController.cpp

HEADERS += \
    $$PWD/controllers/SerialPortController.h \
    $$PWD/controllers/GameController.h \
    $$PWD/domain/Burger.h \
    $$PWD/factories/MessageFactory.hpp \
    $$PWD/helpers/DebugHelper.hpp \
    $$PWD/Constants.h \
    $$PWD/controllers/BurgerController.h \
    $$PWD/Version.h

INCLUDEPATH += \
    $$PWD \
    $$PWD/controllers \
    $$PWD/domain \
    $$PWD/factories \
    $$PWD/helpers

INCLUDEPATH += \
    $$PWD \
    $$PWD/../controllers \
    $$PWD/../domain \
    $$PWD/../factories \
    $$PWD/../helpers
