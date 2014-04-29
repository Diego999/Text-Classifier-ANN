#include "include/NeuralNetwork/anncontroller.h"
#include "include/NeuralNetwork/artificialneuralnetwork.h"
#include "include/Utils/utils.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>

#define DEFAULT_ERROR 15
#define SPACE " "

ANNController::ANNController(int nbInputs, int nbOutputs, const std::vector<int>& nbNeuronsPerHiddenLayer, double learningRate, double momentum):m_stopTraining(false)
{
    createANN(nbInputs, nbOutputs, nbNeuronsPerHiddenLayer, learningRate, momentum);
    m_error = DEFAULT_ERROR;
}

ANNController::ANNController(const ArtificialNeuralNetwork& ann)
{
    createANN(ann);
    m_error = DEFAULT_ERROR;
}

ANNController::ANNController(const ArtificialNeuralNetwork& ann ,const std::vector<std::pair<std::vector<double>, std::vector<double>>>& trainingSet, const std::vector<std::pair<std::vector<double>, std::vector<double>>>& validationSet)
    :ANNController(ann)
{
    m_trainingSet = trainingSet;
    m_validationSet = validationSet;
}

ANNController::ANNController(const std::vector<int>& nbNeuronsPerHiddenLayer, double learningRate, double momentum, const std::vector<std::pair<std::vector<double>, std::vector<double>>>& trainingSet) :
    ANNController(trainingSet[0].first.size(), trainingSet[0].second.size(), nbNeuronsPerHiddenLayer, learningRate, momentum)
{
    m_trainingSet = trainingSet;
}

ANNController::ANNController(const std::vector<int>& nbNeuronsPerHiddenLayer, double learningRate, double momentum, const std::vector<std::pair<std::vector<double>, std::vector<double>>>& trainingSet, const std::vector<std::pair<std::vector<double>, std::vector<double>>>& validationSet) :
    ANNController(nbNeuronsPerHiddenLayer, learningRate, momentum, trainingSet)
{
    m_validationSet = validationSet;
}

double ANNController::kFoldCrossValidation(const std::function<void (long, std::vector<double>&, std::vector<double>&)> &callback, const std::function<void (long, double)> &callbackFinalANN, const unsigned int k)
{
    assert(k >= 0);
    std::vector<std::pair<std::vector<double>, std::vector<double>>> sets;
    ipp_utils::mergeVectors(sets, {m_trainingSet, m_validationSet});

    std::vector<std::vector<std::pair<std::vector<double>, std::vector<double>>>> samples = ipp_utils::createSubSamples(sets, k);
    std::vector<std::vector<std::pair<std::vector<double>, std::vector<double>>>> trainingSets;
    std::vector<std::vector<std::pair<std::vector<double>, std::vector<double>>>> validationSets;
    std::vector<double> errorTraining;
    std::vector<double> errorValidation;

    // Create the k training & validation sets
    for(unsigned int i = 0; i < k; ++i)
    {
        std::vector<std::pair<std::vector<double>, std::vector<double>>> trainingSet;
        std::vector<std::pair<std::vector<double>, std::vector<double>>> validationSet(samples[i]);
        for(unsigned int j = 0; j < k; ++j)
            if(i!=j)
                ipp_utils::mergeVectors(trainingSet, {samples[j]});
        trainingSets.push_back(trainingSet);
        validationSets.push_back(validationSet);
        errorTraining.push_back(0.0);
        errorValidation.push_back(0.0);
    }

    long iteration = 0L;
    double errorTot = 0.0;
    m_stopTraining = false;

    // Train
    do
    {
        for(auto& e:errorTraining)
            e = 0;
        for(auto& e:errorValidation)
            e = 0;

        int k = 0;
        for(auto& trainingSet:trainingSets)
        {
            for(auto& training:trainingSet)
                errorTraining[k] += trainIteration(training);
            ++k;
        }

        k = 0;
        for(auto& validationSet:validationSets)
        {
            for(auto& validate:validationSet)
                errorValidation[k] += validateIteration(validate);
            ++k;
        }

        callback(iteration++, errorTraining, errorValidation);

        errorTot = 0.0;
        // Compute the error over all the validation sets for the final neural network
        for(auto& s:sets)
            errorTot += m_ann->validate(s.first, s.second);

        callbackFinalANN(iteration, errorTot);
    }while(errorTot >= m_error && !m_stopTraining);

    double sum = 0.0;
    //Compute the average of successful
    for(auto& validationSet:validationSets)
        sum += test(validationSet);
    return sum/k;
}

void ANNController::train(const std::function<void(long, double, double)> &callback)
{
    long iteration = 0L;
    double totalCurrentErrorTraining, totalCurrentErrorTest;
    m_stopTraining = false;
    do
    {
        totalCurrentErrorTraining = totalCurrentErrorTest = 0.0;
        size_t j = -1;
        while(++j < m_trainingSet.size())
            totalCurrentErrorTraining += trainIteration(m_trainingSet[j]);
        j = -1;
        while(++j < m_validationSet.size())
            totalCurrentErrorTest += validateIteration(m_validationSet[j]);
        callback(iteration++, totalCurrentErrorTraining, totalCurrentErrorTest);
    } while(totalCurrentErrorTraining >= m_error && !m_stopTraining);
}

double ANNController::trainIteration(const std::pair<std::vector<double>, std::vector<double>>& set)
{
    return m_ann->train(set.first, set.second);
}

double ANNController::validateIteration(const std::pair<std::vector<double>, std::vector<double>>& set)
{
    return m_ann->validate(set.first, set.second);
}

double ANNController::test(const std::vector<std::pair<std::vector<double>, std::vector<double>>>& set) const
{
    double tot = 0;
    for(auto& pair : set)
    {
        std::vector<double> output = m_ann->feedForward(pair.first);
        if(output.size() == pair.second.size())
        {
            auto it1 = output.cbegin();
            auto it2 = pair.second.cbegin();
            bool isOk = true;
            while(it1 != output.cend() && it2 != pair.second.cend() && isOk)
            {
                std::cout << *it1 << " " << *it2 << std::endl;
                if((*it1 < 0.5 && *it2 >= 0.5) || (*it1 >= 0.5 && *it2 < 0.5))
                    isOk = false;
                ++it1;++it2;
            }
            if(isOk)
                ++tot;
        }
    }
    return 100.0*tot/set.size();
}

const std::vector<double>&  ANNController::feedForward(const std::vector<double>& dataInputs)
{
    return m_ann->feedForward(dataInputs);
}

std::vector<std::vector<double>>  ANNController::feedForward(const std::vector<std::vector<double>>& dataInputs)
{
    std::vector<std::vector<double>> results;
    for(size_t i = 0; i < dataInputs.size(); ++i)
        results.push_back(m_ann->feedForward(dataInputs[i]));
    return results;
}

std::vector<double> ANNController::weights(int numLayer, int numNeuron) const
{
    return m_ann->weights(numLayer, numNeuron);
}

void ANNController::createANN(int nbInputs, int nbOutputs, const std::vector<int>& nbNeuronsPerHiddenLayer, double learningRate, double momentum)
{
    m_ann = std::make_shared<ArtificialNeuralNetwork>(nbInputs, nbOutputs, nbNeuronsPerHiddenLayer, learningRate, momentum);
}

void ANNController::createANN(const ArtificialNeuralNetwork& ann)
{
    m_ann = std::make_shared<ArtificialNeuralNetwork>(ann);
}
