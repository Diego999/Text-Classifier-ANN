#include "include/Tests/tests.h"
#include "include/NeuralNetwork/artificialneuralnetwork.h"
#include "include/NeuralNetwork/anncontroller.h"
#include "include/Utils/utils.h"
#include "include/Text/textcontroller.h"

#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <cmath>
#include <utility>
#include <sstream>
#include <regex>

using namespace std;

struct TestSet{
    std::string op;
    double input1, input2;
    double target;
};


void testTextController()
{
    TextController tc("../Text-Classifier-ANN/data/tagged/");
    tc.addSubFolder("neg");
    tc.addSubFolder("pos");

    tc.addTag("NOM");
    tc.addTag("VER");
    tc.addTag("ADJ");

    vector<pair<map<string, int>,double>> globalMap;
    map<string, int> map1;
    map<string, int> map2;

    map1["A"]=1;
    map1["B"]=2;
    map1["C"]=3;
    map1["D"]=4;

    map2["W"]=11;
    map2["X"]=12;
    map2["Y"]=13;
    map2["Z"]=14;

    globalMap.push_back(make_pair(map1, 0.1));
    globalMap.push_back(make_pair(map2, 0.9));

    std::vector<std::pair<std::vector<double>, std::vector<double> > > v = tc.createGlobalVector(globalMap);
    cout << "Size of vectors is : " << v[0].first.size() << std::endl;
    cout << "vect:\t[0]\t[1]" << endl;
    cout << "score:\t"<< v[0].second[0] <<"\t"<< v[1].second[0] << endl;

    for(int i = 0; i < v[0].first.size(); ++i)
    {
        cout << "word " << i << "\t" << v[0].first[i] << "\t" << v[1].first[i] << endl;
    }
}

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

void testKFoldCrossValidation()
{
    std::vector<std::pair<std::vector<double>, std::vector<double>>> trainingSet;
    trainingSet.push_back(std::pair<std::vector<double>, std::vector<double>>({0.0, 0.0}, {0.1}));
    trainingSet.push_back(std::pair<std::vector<double>, std::vector<double>>({1.0, 1.0}, {0.1}));

    std::vector<std::pair<std::vector<double>, std::vector<double>>> validationSet;
    validationSet.push_back(std::pair<std::vector<double>, std::vector<double>>({1.0, 0.0}, {0.9}));
    validationSet.push_back(std::pair<std::vector<double>, std::vector<double>>({0.0, 1.0}, {0.9}));

    ANNController ann({3}, 0.1, 0.1, trainingSet, validationSet);
    ann.kFoldCrossValidation(
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
    2
    );

}
