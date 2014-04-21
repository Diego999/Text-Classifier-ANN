#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <vector>

class SettingsNeuralNetwork
{
public:
    static const int nbOutputs;
    static const int nbHiddenLayers;
    static const int nbNeuronsPerHiddenLayer;

    static const double learningRate;
    static const double momentum;

    static const std::vector<std::string> tags;
};

#endif // SETTINGS_H
