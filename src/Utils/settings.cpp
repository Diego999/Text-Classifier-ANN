#include "include/Utils/settings.h"

const int SettingsNeuralNetwork::nbOutputs = 2;
const int SettingsNeuralNetwork::nbHiddenLayers = 1;
const int SettingsNeuralNetwork::nbNeuronsPerHiddenLayer = 3;

const double SettingsNeuralNetwork::learningRate = 0.1;
const double SettingsNeuralNetwork::momentum = 0.3;

const std::vector<std::string> SettingsNeuralNetwork::tags = {"ADJ",
                                                              "NOM",
                                                              "VER:cond", "VER:futu", "VER:impe", "VER:impf", "VER:infi", "VER:pper", "VER:ppre", "VER:pres", "VER:simp", "VER:subi", "VER:subp"};
