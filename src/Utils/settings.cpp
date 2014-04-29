#include "include/Utils/settings.h"

const int SettingsNeuralNetwork::nbOutputs = 2;
const int SettingsNeuralNetwork::nbHiddenLayers = 2;
const int SettingsNeuralNetwork::nbNeuronsPerHiddenLayer = 15;

const double SettingsNeuralNetwork::learningRate = 0.2;
const double SettingsNeuralNetwork::momentum = 0.2;

const std::vector<std::string> SettingsNeuralNetwork::tags = {"ADJ",
                                                              "NOM",
                                                              "VER:cond", "VER:futu", "VER:impe", "VER:impf", "VER:infi", "VER:pper", "VER:ppre", "VER:pres", "VER:simp", "VER:subi", "VER:subp"};
