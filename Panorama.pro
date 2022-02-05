QT += gui

CONFIG += c++11 console
CONFIG -= app_bundle

include(blur/blur.pri)
include(utils/utils.pri)
include(Direction/Direction.pri)
include(Moravec/Moravec.pri)
include(NonMaximalSuppression/NonMaximalSuppression.pri)
include(Descriptor/Descriptor.pri)
include(Detector/Detector.pri)

SOURCES += \
        ImageCombiner.cpp \
        main.cpp

HEADERS += \
  ImageCombiner.h \
  aliases.h
