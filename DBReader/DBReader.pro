include (../pconfig.pri)

ConsoleApplication(dbreader)
ImportModules(Foundation Util XML JSON, Poco)
SOURCES += main.cpp \
    ComandReader.cpp

HEADERS += \
    ComandReader.h
