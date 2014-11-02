TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += AntTweakBar/include/

LIBS += -lAntTweakBar -lglut -lGL -lX11 -lGLEW

SOURCES += main.cpp \
    shader.cpp \
    model.cpp

HEADERS += \
    common.h \
    shader.h \
    AntTweakBar/include/AntTweakBar.h \
    model.h

OTHER_FILES += \
    model.obj \
    shaders/0.glslfs \
    shaders/0.glslvs

