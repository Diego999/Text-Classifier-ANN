#include "include/NeuralNetwork/anncontroller.h"
#include "include/NeuralNetwork/artificialneuralnetwork.h"
#include "include/Utils/utils.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>

#define DEFAULT_ERROR 0.001
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

ANNController::ANNController(const std::string& filepath):m_stopTraining(false)
{
    importANN(filepath);
    m_error = DEFAULT_ERROR;
}

ANNController::ANNController(const std::string& filepath, const std::vector<std::pair<std::vector<double>, std::vector<double>>>& trainingSet) :
    ANNController(filepath)
{
    m_trainingSet = trainingSet;
}

ANNController::ANNController(const std::string& filepath, const std::vector<std::pair<std::vector<double>, std::vector<double>>>& trainingSet, const std::vector<std::pair<std::vector<double>, std::vector<double>>>& validationSet) :
    ANNController(filepath, trainingSet)
{
    m_validationSet = validationSet;
}

void ANNController::kFoldCrossValidation(const std::function<void (long, std::vector<double>, std::vector<double>)> &callback, const std::function<void (long, double)> &callbackFinalANN, const unsigned int k)
{
    assert(k >= 0);
    std::vector<std::pair<std::vector<double>, std::vector<double>>> sets;
    ipp_utils::mergeVectors(sets, {m_trainingSet, m_validationSet});

    std::vector<std::vector<std::pair<std::vector<double>, std::vector<double>>>> samples = ipp_utils::createSubSamples(sets, k);

    std::vector<ANNController> annControllers;
    std::vector<double> errorTraining;
    std::vector<double> errorValidation;
    std::vector<std::shared_ptr<ArtificialNeuralNetwork>> anns;
    for(unsigned int i = 0; i < k; ++i)
    {
        std::vector<std::pair<std::vector<double>, std::vector<double>>> trainingSet;
        std::vector<std::pair<std::vector<double>, std::vector<double>>> validationSet(samples[i]);
        for(unsigned int j = 0; j < k; ++j)
            if(i!=j)
                ipp_utils::mergeVectors(trainingSet, {samples[i]});
        annControllers.push_back(ANNController(*m_ann, trainingSet, validationSet));
        anns.push_back(annControllers.back().m_ann);
        errorTraining.push_back(0.0);
        errorValidation.push_back(0.0);
    }

    long iteration = 0L;
    double errorTot;
    m_stopTraining = false;
    do
    {
        for(auto& e:errorTraining)
            e = 0.0;
        for(auto& e:errorValidation)
            e = 0.0;
        for(unsigned int i = 0; i < (sets.size()/k + sets.size()%k); ++i)
        {
            errorTot = 0.0;
            for(auto& ann:annControllers)
            {
                if(i < ann.m_trainingSet.size())
                    errorTraining[i] = ann.trainIteration(ann.m_trainingSet[i]);
                if(i < ann.m_validationSet.size())
                    errorValidation[i] = ann.validateIteration(ann.m_validationSet[i]);
            }
            callback(iteration, errorTraining, errorValidation);
            ArtificialNeuralNetwork annNew(anns);
            for(auto& s:sets)
                errorTot += annNew.validate(s.first, s.second);
            callbackFinalANN(iteration, errorTot);
        }
        ++iteration;
    }while(errorTot >= m_error && !m_stopTraining);
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

void ANNController::exportANN(const std::string& filepath)
{
    std::ofstream file(filepath);
    file << m_ann->nbInputs() << SPACE << m_ann->nbOutputs() << SPACE << m_ann->nbHiddenLayers() << SPACE;
    for(auto& nbNeurons : m_ann->nbNeuronsPerHiddenLayer())
        file << nbNeurons << SPACE;
    file << m_ann->learningRate() << SPACE << m_ann->momentum() << SPACE;
    for(int i = 0; i < m_ann->nbHiddenLayers() + 1; ++i)
    {
        int nbNeurons = (i < m_ann->nbHiddenLayers()) ? m_ann->nbNeuronsPerHiddenLayer()[i] : m_ann->nbOutputs();
        for(int j = 0; j < nbNeurons; ++j)
        {
            for(auto& weight : m_ann->weights(i,j))
                file << weight << SPACE;
            file << m_ann->threshold(i, j) << SPACE << m_ann->deltaNeuron(i, j) << SPACE;
            for(auto& prevWeight : m_ann->prevDeltaWeights(i,j))
                file << prevWeight << SPACE;
            file << m_ann->prevDeltaThreshold(i, j) << SPACE;
        }
    }
    file.close();
}

void ANNController::importANN(const std::string& filepath)
{
    std::ifstream rfile(filepath);
    std::stringstream buffer;
    buffer << rfile.rdbuf();
    std::string content(buffer.str());
    std::string::size_type fullIdx = 0;
    std::string::size_type idx = 0;
    int nbInputs = std::stoi(content, &idx);
    fullIdx += idx;
    int nbOutputs = std::stoi(content.substr(fullIdx), &idx);
    fullIdx += idx;
    int nbHiddenLayers = std::stoi(content.substr(fullIdx), &idx);
    fullIdx += idx;
    std::vector<int> nbNeuronsPerHiddenLayer;
    for(int i = 0; i < nbHiddenLayers; ++i)
    {
        int nbNeurons = std::stoi(content.substr(fullIdx), &idx);
        fullIdx += idx;
        nbNeuronsPerHiddenLayer.push_back(nbNeurons);
    }

    double learningRate = std::stod(content.substr(fullIdx), &idx);
    fullIdx += idx;
    double momentum = std::stod(content.substr(fullIdx), &idx);
    fullIdx += idx;
    createANN(nbInputs, nbOutputs, nbNeuronsPerHiddenLayer, learningRate, momentum);

    for(int i = 0; i < nbHiddenLayers + 1; ++i)
    {
        int nbNeurons = (i < nbHiddenLayers) ? nbNeuronsPerHiddenLayer[i] : nbOutputs;
        for(int j = 0; j < nbNeurons; ++j)
        {
            size_t nbNeuronsParent = (i > 0) ? nbNeuronsPerHiddenLayer[i-1] : nbInputs;
            for(size_t k = 0; k < nbNeuronsParent; ++k)
            {
                double weight = std::stod(content.substr(fullIdx), &idx);
                fullIdx += idx;
                m_ann->weight(i, j, k, weight);
            }
            double threshold = std::stod(content.substr(fullIdx), &idx);
            fullIdx += idx;
            m_ann->threshold(i, j, threshold);
            double deltaNeuron = std::stod(content.substr(fullIdx), &idx);
            fullIdx += idx;
            m_ann->deltaNeuron(i, j, deltaNeuron);
            for(size_t k = 0; k < nbNeuronsParent; ++k)
            {
                double prevDeltaWeight = std::stod(content.substr(fullIdx), &idx);
                fullIdx += idx;
                m_ann->prevDeltaWeight(i, j, k, prevDeltaWeight);
            }
            double prevDeltaThreshold = std::stod(content.substr(fullIdx), &idx);
            fullIdx += idx;
            m_ann->prevDeltaThreshold(i, j, prevDeltaThreshold);
        }
    }
}

void ANNController::createANN(int nbInputs, int nbOutputs, const std::vector<int>& nbNeuronsPerHiddenLayer, double learningRate, double momentum)
{
    m_ann = std::make_shared<ArtificialNeuralNetwork>(nbInputs, nbOutputs, nbNeuronsPerHiddenLayer, learningRate, momentum);
}

void ANNController::createANN(const ArtificialNeuralNetwork& ann)
{
    m_ann = std::make_shared<ArtificialNeuralNetwork>(ann);
}

std::string ANNController::log() const
{
    std::stringstream log;
    log << "[START]" << std::endl;
    log << "Nb inputs: " << m_ann->nbInputs() << std::endl;
    log << "Nb outputs: " << m_ann->nbOutputs() << std::endl;
    log << "Nb hidden layers: " << m_ann->nbHiddenLayers() << std::endl;
    log << "Nb neurons per hidden layouts: {";
    for(auto& nbNeurons : m_ann->nbNeuronsPerHiddenLayer())
        log << nbNeurons << ", ";
    log << "}" << std::endl;
    log << "Learning rate: " << m_ann->learningRate() << std::endl;
    log << "Momentum: " << m_ann->momentum() << std::endl;
    for(int i = 0; i < m_ann->nbHiddenLayers() + 1; ++i)
    {
        int nbNeurons = (i < m_ann->nbHiddenLayers()) ? m_ann->nbNeuronsPerHiddenLayer()[i] : m_ann->nbOutputs();
        for(int j = 0; j < nbNeurons; ++j)
        {
            log << "---------------------------------" << std::endl;
            log << "Neuron " << j << " in layer " << i << ":" << std::endl;
            log << "---------------------------------" << std::endl;
            log << "Weights :";
            for(auto& weight : m_ann->weights(i,j))
                log << SPACE << weight;
            log << std::endl;
            log << "Threshold: " << m_ann->threshold(i, j) << std::endl;
            log << "Delta: " << m_ann->deltaNeuron(i, j) << std::endl;
            log << "Previous weights :";
            for(auto& prevWeight : m_ann->prevDeltaWeights(i,j))
                log << SPACE << prevWeight;
            log << std::endl;
            log << "Previous delta threshold: " << m_ann->prevDeltaThreshold(i, j) << std::endl;
        }
    }
    log << "---------------------------------" << std::endl;
    log << "[END]" << std::endl;
    return log.str();
}
