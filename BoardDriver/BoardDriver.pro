include(../pconfig.pri)

ConsoleApplication(boarddriver)
ImportModules(Foundation Util XML JSON, Poco)

SOURCES += main.cpp \
    DisplayModule.cpp \
    InfoBoard.cpp \
    RS232.cpp

HEADERS += \
    DisplayModule.h \
    InfoBoard.h \
    RS232.h

DEFINES += __LINUX__

DISTFILES += \
    pconfig.pri
