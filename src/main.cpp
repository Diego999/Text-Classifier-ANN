#include "include/Utils/settings.h"
#include "include/Text/textcontroller.h"
#include "include/NeuralNetwork/anncontroller.h"
#include <iostream>
#include <algorithm>

int main(int argc, char* argv[])
{
    TextController tc("../Text-Classifier-ANN/data/tagged/");
    tc.addSubFolder("neg");
    tc.addSubFolder("pos");

    for(auto& tag : SettingsNeuralNetwork::tags)
    {
        tc.addTag(tag);
    }

    std::vector<std::pair<std::vector<double>,std::vector<double>>> globalVector = tc.getGlobalVector();
    std::random_shuffle(globalVector.begin(),globalVector.end());
    std::cout << "Size of globalVector is : " << globalVector.size() << std::endl;

    std::vector<std::pair<std::vector<double>,std::vector<double>>> training(globalVector.begin(),globalVector.begin()+(80*globalVector.size()/100));
    std::vector<std::pair<std::vector<double>,std::vector<double>>> validation(globalVector.begin()+(80*globalVector.size()/100)+1,globalVector.end());

    ANNController annController({SettingsNeuralNetwork::nbNeuronsPerHiddenLayer},
                                SettingsNeuralNetwork::learningRate,
                                SettingsNeuralNetwork::momentum,
                                training,validation);

    std::cout << "Starting training" << std::endl;

    // Uses standard training (1 trainingSet, 1 validationSet)
    //annController.train([&](long iteration, double trainingError, double testingError){std::cout << "Total error : " << (trainingError + testingError) << std::endl;});

    // Uses k Fold Cross Validation
    const unsigned int k = 2;
    annController.kFoldCrossValidation(
                [&](long i, std::vector<double> &errT, std::vector<double>& errV)
                {
                    auto it1 = errT.begin();
                    auto it2 = errV.begin();

                    while(it1 != errT.end() && it2 != errV.end())
                    {
                        std::cout << i << " " << *it1 << " " << *it2 << std::endl;
                        ++it1;
                        ++it2;
                    }
                },
                [&](long i, double err)
                    {
                        std::cout << "\t" << i << " " << err << std::endl << std::endl;
                    },
                k
                );


    std::cout << std::endl << "Successfulness : " << annController.test(validation) << std::endl;

    return 0;
}
