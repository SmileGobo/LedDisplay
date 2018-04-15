TEMPLATE = subdirs

SUBDIRS += \
    DBReader \
    BoardDriver

BoardDriver.depends += DBReader
