#ifndef ANNCONTROLLER_H
#define ANNCONTROLLER_H

#include <vector>
#include <string>
#include <memory>
#include <functional>

class ArtificialNeuralNetwork;

class ANNController
{
private:
    ANNController(int nbInputs, int nbOutputs, const std::vector<int>& nbNeuronsPerHiddenLayer, double learningRate, double momentum);
    ANNController(const ArtificialNeuralNetwork& ann);
    ANNController(const ArtificialNeuralNetwork& ann ,const std::vector<std::pair<std::vector<double>, std::vector<double>>>& trainingSet, const std::vector<std::pair<std::vector<double>, std::vector<double>>>& validationSet);
public:
    ANNController(const std::vector<int>& nbNeuronsPerHiddenLayer, double learningRate, double momentum, const std::vector<std::pair<std::vector<double>, std::vector<double>>>& trainingSet);
    ANNController(const std::vector<int>& nbNeuronsPerHiddenLayer, double learningRate, double momentum, const std::vector<std::pair<std::vector<double>, std::vector<double>>>& trainingSet, const std::vector<std::pair<std::vector<double>, std::vector<double>>>& validationSet);
    ANNController(const std::string& filepath);
    ANNController(const std::string& filepath, const std::vector<std::pair<std::vector<double>, std::vector<double>>>& trainingSet);
    ANNController(const std::string& filepath, const std::vector<std::pair<std::vector<double>, std::vector<double>>>& trainingSet, const std::vector<std::pair<std::vector<double>, std::vector<double>>>& validationSet);

    void train(const std::function<void(long, double, double)> &callback);
    double test(const std::vector<std::pair<std::vector<double>, std::vector<double>>>& set) const;
    void kFoldCrossValidation(const std::function<void (long, std::vector<double>&, std::vector<double>&)> &callback, const std::function<void (long, double)> &callbackFinalANN, const unsigned int k);
    void stopTraining() {m_stopTraining = true;}

    const std::vector<double>& feedForward(const std::vector<double>& dataInputs);
    std::vector<std::vector<double>> feedForward(const std::vector<std::vector<double>>& dataInputs);
    std::vector<double> weights(int numLayer, int numNeuron) const;
    void exportANN(const std::string& filepath);

    double error() const { return m_error; }

    void error(double error) { m_error = error; }

    std::string log() const;

private:
    void importANN(const std::string& filepath);
    void createANN(int nbInputs, int nbOutputs, const std::vector<int>& nbNeuronsPerHiddenLayer, double learningRate, double momentum);
    void createANN(const ArtificialNeuralNetwork& ann);

    double trainIteration(const std::pair<std::vector<double>, std::vector<double>>& set);
    double validateIteration(const std::pair<std::vector<double>, std::vector<double>>& set);

    std::shared_ptr<ArtificialNeuralNetwork> m_ann;

    std::vector<std::pair<std::vector<double>, std::vector<double>>> m_trainingSet;
    std::vector<std::pair<std::vector<double>, std::vector<double>>> m_validationSet;

    double m_error;
    bool m_stopTraining;
};

#endif // ANNCONTROLLER_H
