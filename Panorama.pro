QT += gui testlib

CONFIG += c++17 console
CONFIG -= app_bundle

include(utils/utils.pri)
include(testUtils/testUtils.pri)

include(blur/blur.pri)
include(FAST/FAST.pri)
include(BRIEF/BRIEF.pri)
include(RANSAC/RANSAC.pri)
include(Combine/Combine.pri)

SOURCES += \
        main.cpp

HEADERS += \
  aliases.h
