TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += AntTweakBar/include/

LIBS += -lAntTweakBar -lglut -lGL -lX11 -lGLEW

SOURCES += main.cpp \
    shader.cpp

HEADERS += \
    common.h \
    shader.h \
    AntTweakBar/include/AntTweakBar.h

OTHER_FILES += \
    shaders/3.glslfs \
    shaders/3.glslvs \
    shaders/2.glslfs \
    shaders/2.glslvs

