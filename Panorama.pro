QT += gui testlib

CONFIG += c++17 console
CONFIG -= app_bundle

include(blur/blur.pri)
include(utils/utils.pri)
include(testUtils/testUtils.pri)
include(Direction/Direction.pri)
include(Moravec/Moravec.pri)
include(NonMaximalSuppression/NonMaximalSuppression.pri)
include(Descriptor/Descriptor.pri)
include(Detector/Detector.pri)
include(Optimization/Optimization.pri)
include(FAST/FAST.pri)
include(BRIEF/BRIEF.pri)
include(RANSAC/RANSAC.pri)
include(Combine/Combine.pri)

SOURCES += \
        ImageCombiner.cpp \
        main.cpp

HEADERS += \
  ImageCombiner.h \
  aliases.h
