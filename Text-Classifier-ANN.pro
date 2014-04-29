QT       += core gui widgets

TARGET = Test
TEMPLATE = app


macx {
    QMAKE_CXXFLAGS = -mmacosx-version-min=10.7 -std=gnu0x -stdlib=libc+
}

CONFIG +=c++11
CONFIG +=console

HEADERS += \
    include/NeuralNetwork/anncontroller.h \
    include/NeuralNetwork/artificialneuralnetwork.h \
    include/NeuralNetwork/neuron.h \
    include/NeuralNetwork/neuronlayer.h \
    include/Utils/settings.h \
    include/Utils/utils.h \
    include/Tests/tests.h \
    include/Text/textcontroller.h

SOURCES += \
    src/NeuralNetwork/anncontroller.cpp \
    src/NeuralNetwork/artificialneuralnetwork.cpp \
    src/NeuralNetwork/neuron.cpp \
    src/NeuralNetwork/neuronlayer.cpp \
    src/Tests/tests.cpp \
    src/Utils/settings.cpp \
    src/Utils/utils.cpp \
    src/main.cpp \
    src/Text/textcontroller.cpp
