QT      += core gui widgets

CONFIG  += c++17 console
CONFIG  -= app_bundle

INCLUDEPATH += src/

SOURCES += \
        src/main.cpp \
        src/MainWindow.cpp \
        src/blur/blur.cpp \
        src/BRIEF/BRIEF.cpp \
        src/FAST/FAST.cpp \
        src/RANSAC/RANSAC.cpp \
        src/Combine/Combine.cpp \
        src/utils/utils.cpp

HEADERS += \
        src/MainWindow.h \
        src/aliases.h \
        src/blur/blur.h \
        src/BRIEF/BRIEF.h \
        src/FAST/FAST.h \
        src/RANSAC/RANSAC.h \
        src/Combine/Combine.h \
        src/utils/utils.h
