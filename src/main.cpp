#include "include/Utils/settings.h"
#include "include/Text/textcontroller.h"
#include "include/NeuralNetwork/anncontroller.h"
#include <iostream>
#include <algorithm>

int main(int argc, char* argv[])
{
    // Parameters
    bool useKfoldCrossValidation = true;
    bool useCanonicForm = true;
    bool verbose = true;
    const unsigned int k = 5;

    TextController tc("../Text-Classifier-ANN/data/tagged/");
    tc.addSubFolder("neg");
    tc.addSubFolder("pos");

    for(auto& tag : SettingsNeuralNetwork::tags)
    {
        tc.addTag(tag);
    }

    // Algorithms
    std::vector<std::pair<std::vector<double>,std::vector<double>>> globalVector = tc.getGlobalVector(useCanonicForm, verbose);
    std::random_shuffle(globalVector.begin(),globalVector.end());
    if(verbose) std::cout << "Size of globalVector is : " << globalVector.size() << std::endl;

    std::vector<std::pair<std::vector<double>,std::vector<double>>> training(globalVector.begin(),globalVector.begin()+(80*globalVector.size()/100));
    std::vector<std::pair<std::vector<double>,std::vector<double>>> validation(globalVector.begin()+(80*globalVector.size()/100)+1,globalVector.end());

    ANNController annController({SettingsNeuralNetwork::nbNeuronsPerHiddenLayer},
                                SettingsNeuralNetwork::learningRate,
                                SettingsNeuralNetwork::momentum,
                                training,validation);

    if(useKfoldCrossValidation)
    {
        if(verbose)
        {
            std::cout << "Starting k fold cross validation with k = " << k << std::endl;
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
        }
        else
        {
            annController.kFoldCrossValidation([&](long i, std::vector<double> &errT, std::vector<double>& errV){;},[&](long i, double err){;},k);
        }
    }
    else
    {
        if(verbose)
        {
            std::cout << "Starting strandard training" << std::endl;
            annController.train([&](long iteration, double trainingError, double testingError){std::cout << "Total error : " << "i " <<iteration<<" "<< (trainingError + testingError) << std::endl;});
        }
        else
        {
            annController.train([&](long iteration, double trainingError, double testingError){;});
        }
    }
    std::cout << std::endl << "Successfulness : " << annController.test(validation) << std::endl;
    return 0;
}
