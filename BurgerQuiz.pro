SOURCES += \
        main.cpp \    
    controllers/FormLog.cpp

HEADERS += \
    controllers/FormLog.h

RESOURCES += qml.qrc

DISTFILES += \
    BurgerQuiz.pri

include(BurgerQuiz.pri)

FORMS += \
    controllers/FormLog.ui
