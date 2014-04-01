#include "include/Tests/tests.h"
#include "include/NeuralNetwork/artificialneuralnetwork.h"
#include "include/NeuralNetwork/anncontroller.h"
#include "include/Utils/utils.h"

#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <cmath>

struct TestSet{
    std::string op;
    double input1, input2;
    double target;
};

void testsANN()
{
    std::vector<TestSet> testSets;

    testSets.push_back(TestSet{"AND",0,0,0.1});
    testSets.push_back(TestSet{"AND",0,1,0.1});
    testSets.push_back(TestSet{"AND",1,0,0.1});
    testSets.push_back(TestSet{"AND",1,1,0.9});

    testSets.push_back(TestSet{"NAND",0,0,0.9});
    testSets.push_back(TestSet{"NAND",0,1,0.9});
    testSets.push_back(TestSet{"NAND",1,0,0.9});
    testSets.push_back(TestSet{"NAND",1,1,0.1});

    testSets.push_back(TestSet{"OR",0,0,0.1});
    testSets.push_back(TestSet{"OR",0,1,0.9});
    testSets.push_back(TestSet{"OR",1,0,0.9});
    testSets.push_back(TestSet{"OR",1,1,0.9});

    testSets.push_back(TestSet{"NOR",0,0,0.9});
    testSets.push_back(TestSet{"NOR",0,1,0.1});
    testSets.push_back(TestSet{"NOR",1,0,0.1});
    testSets.push_back(TestSet{"NOR",1,1,0.1});

    testSets.push_back(TestSet{"XOR",0,0,0.1});
    testSets.push_back(TestSet{"XOR",0,1,0.9});
    testSets.push_back(TestSet{"XOR",1,0,0.9});
    testSets.push_back(TestSet{"XOR",1,1,0.1});

    testSets.push_back(TestSet{"NXOR",0,0,0.9});
    testSets.push_back(TestSet{"NXOR",0,1,0.1});
    testSets.push_back(TestSet{"NXOR",1,0,0.1});
    testSets.push_back(TestSet{"NXOR",1,1,0.9});

    int k = 0;
    for(size_t i = 0; i < testSets.size(); i+=4)
    {
        ArtificialNeuralNetwork ann(2, 1, {3}, 0.1, 0.1);
        double err = 0;
        size_t j = 0;
        k=0;
        do
        {
            j = 0;
            err = 0;
            while(j++<4)
            {
                TestSet ts = testSets[i+j%4];
                err += ann.train({ts.input1, ts.input2}, {ts.target});
            }
            ++k;
        }while(err >= 0.001);

        for(j = i; j < i+4; ++j)
        {
            TestSet ts = testSets[j];
            double out = ann.feedForward({ts.input1, ts.input2})[0];
            std::cout << static_cast<int>(ts.input1) << " " << ts.op << " " << static_cast<int>(ts.input2) << " -> " << out << " (" << ts.target << ")" << std::endl;
            assert(fabs(out-ts.target) < 0.4);
        }
        std::cout << "Error : " << err << std::endl << "iterations " << k << std::endl << std::endl;
    }
}

void testsANNController()
{
    std::string filepath = "testsANNController.ann";
    std::vector<std::pair<std::vector<double>, std::vector<double>>> testSets;

    testSets.push_back({{0,0},{0.1}});
    testSets.push_back({{0,1},{0.9}});
    testSets.push_back({{1,0},{0.9}});
    testSets.push_back({{1,1},{0.1}});
    ANNController* annc = new ANNController({3}, 0.5, 0.5,testSets, testSets);
    annc->error(0.01);
    std::cout << "Lambda function: " << std::endl;
    std::function<void(long, double, double)> callback = [&](long iteration, double trainingError, double testingError)
    {
        std::cout << "iteration: " << iteration << " " << trainingError << " - " << testingError << std::endl;
    };
    std::vector<std::vector<double>> validationSets;

    validationSets.push_back({0,0});
    validationSets.push_back({0,1});
    validationSets.push_back({1,0});
    validationSets.push_back({1,1});

    annc->train(callback);
    annc->exportANN(filepath);
    for(auto& result : annc->feedForward(validationSets))
        std::cout << result[0] << std::endl;

    ANNController* annc2 = new ANNController(filepath);
    for(auto& result : annc2->feedForward(validationSets))
        std::cout << result[0] << std::endl;

    std::cout << annc->log();
    std::cout << annc2->log();
    delete annc;
    delete annc2;
}

void testsANNMerge()
{
    ArtificialNeuralNetwork a1(2, 1, {1}, 0.1, 0.2);
    ArtificialNeuralNetwork a2(2, 1, {1}, 0.3, 0.4);
    a1.weight(0, 0, 0, 0.5);a1.weight(0, 0, 1, 1.0);a1.weight(1, 0, 0, 1.5);a1.threshold(0, 0, 2.0);a1.threshold(1, 0, 2.5);
    a1.prevDeltaWeight(0, 0, 0, 0.5);a1.prevDeltaWeight(0, 0, 1, 1.0);a1.prevDeltaWeight(1, 0, 0, 1.5);a1.prevDeltaThreshold(0, 0, 2.0);a1.prevDeltaThreshold(1, 0, 2.5);
    a2.weight(0, 0, 0, 3.0);a2.weight(0, 0, 1, 3.5);a2.weight(1, 0, 0, 4.0);a2.threshold(0, 0, 4.5);a2.threshold(1, 0, 5.0);
    a2.prevDeltaWeight(0, 0, 0, 3.0);a2.prevDeltaWeight(0, 0, 1, 3.5);a2.prevDeltaWeight(1, 0, 0, 4.0);a2.prevDeltaThreshold(0, 0, 4.5);a2.prevDeltaThreshold(1, 0, 5.0);

    ArtificialNeuralNetwork ann({a1,a2});
    assert(ipp_utils::compare(ann.threshold(0, 0), 3.25));
    assert(ipp_utils::compare(ann.threshold(1, 0), 3.75));
    assert(ipp_utils::compare(ann.weight(0, 0, 0), 1.75));
    assert(ipp_utils::compare(ann.weight(0, 0, 1), 2.25));
    assert(ipp_utils::compare(ann.weight(1, 0, 0), 2.75));
    assert(ipp_utils::compare(ann.prevDeltaThreshold(0, 0), 0.0));
    assert(ipp_utils::compare(ann.prevDeltaThreshold(1, 0), 0.0));
    assert(ipp_utils::compare(ann.prevDeltaWeight(0, 0, 0), 0.0));
    assert(ipp_utils::compare(ann.prevDeltaWeight(0, 0, 1), 0.0));
    assert(ipp_utils::compare(ann.prevDeltaWeight(1, 0, 0), 0.0));
    assert(ipp_utils::compare(ann.learningRate(), 0.1));
    assert(ipp_utils::compare(ann.momentum(), 0.2));
}

void testMergeVector()
{
    std::vector<int> v({1,2,3,4,5,6,7,8,9,10});
    std::vector<std::vector<int>> v2({{11,12}, {13,14}, {15, 16, 17, 18, 19}, {20}});
    ipp_utils::mergeVectors(v, v2);
    for(int i = 0; i < 20; ++i)
        assert(v[i] == (i+1));
}

void testCreateSubSamples()
{
    for(int s = 1; s <= 20; ++s)
    {
        std::vector<int> sets;
        for(int ss = 1; ss <= s; ++ss)
            sets.push_back(ss);
        for(int k = 1; k <= s; ++k)
        {
            int i = 1;
            for(auto& v:ipp_utils::createSubSamples(sets, k))
            {
                assert(v.size() < 2*(sets.size()/k));
                for(auto& vv:v)
                    assert(vv == i++);
            }
        }
    }
}
