QT += gui

CONFIG += c++11 console
CONFIG -= app_bundle

include(blur/blur.pri)
include(utils/utils.pri)
include(Direction/Direction.pri)
include(Moravec/Moravec.pri)
include(NonMaximalSuppression/NonMaximalSuppression.pri)

SOURCES += \
        Detector.cpp \
        main.cpp

HEADERS += \
  Detector.h \
  aliases.h
