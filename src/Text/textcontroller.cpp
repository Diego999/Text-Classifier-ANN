#include "include/Text/textcontroller.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <regex>
#include <sstream>
#include <utility>

using namespace std;

const std::pair<std::string, double> TextController::posFolder = make_pair("pos/",0.9);
const std::pair<std::string, double> TextController::negFolder = make_pair("neg/",0.1);


TextController::TextController(const std::string& dataFolderPath):
    dataFolderPath(dataFolderPath), dataFilesIndex("files.index")
{
}

const std::vector<std::pair<std::vector<double>, std::vector<double> > > TextController::getGlobalVector(const bool& canonic,const bool& verbose)
{
    if(verbose)
    {
        for (string& folderName : dataSubFolder)
            cout << endl << "Import files from folder " << dataFolderPath + folderName;
    }
    // Imports files to vector of string
    std::vector<std::pair<std::string,double>> files = importFiles();
    if(verbose)cout << endl << files.size() <<  " files imported" << endl;
    // Creates histograms for each file
    vector<pair<map<string, int>,double>> map = createGlobalMap(files, canonic);
    if(verbose)cout <<"Starting vector creation (takes a few minutes)" << endl;
    // Creates the global vector
    return createGlobalVector(map);
}

void TextController::addSubFolder(const string &folderName)
{
    dataSubFolder.push_back(folderName);
}

void TextController::addTag(const string &tag)
{
    tagList.push_back(tag);
}

const std::vector<std::pair<std::string,double>> TextController::importFiles()
{
    vector<pair<string,double>> files;
    for (string& folderName : dataSubFolder)
    {
        getFiles(files, (folderName + "/") );
    }
    return files;
}

void TextController::getFiles(std::vector<std::pair<string,double>>& files, const string &prefix)
{
    string path = dataFolderPath + prefix + dataFilesIndex;
    string line;
    double score;
    // Reads each file indexed in the dataFileIndex
    ifstream file (path);
    if (file.is_open())
    {
        // Sets the score of the file 0.9 if po 0.1 if neg
        if(prefix==posFolder.first)
            score = posFolder.second;
        else
            score = negFolder.second;
        // Reads each file
        while ( getline (file,line) )
        {
            files.push_back(make_pair(readFile(dataFolderPath + prefix + line),score));
        }
    }
    file.close();
}

const std::string TextController::readFile(const string& path)
{
    // Appends each line to the strFile
    string line;
    string strFile = "";
    ifstream file (path);
    if (file.is_open())
    {
         while ( getline (file,line) )
         {
            if (keepLine(line))
            {
                strFile.append(line);
                strFile.append("\n");
            }
         }
        file.close();
    }
    file.close();
    return strFile;
}

 bool TextController::keepLine(const std::string& line)
 {
     bool flag = false;
     for (string& tag : tagList)
     {
         // keeps line if tag found on the line
         std::size_t found = line.find(tag);
         if (found!=std::string::npos)
         {
            flag = true;
            break;
         }
     }
     return flag;
 }


const std::vector<std::pair<std::map<std::string, int>,double>> TextController::createGlobalMap(const std::vector<std::pair<std::string,double>>& fileVector, const bool& canonic)
{
    std::vector<std::pair<std::map<std::string, int>,double>> vectorOfMap;
    map<string, int> localMap;
    string line;

    // Turns the string file to an histogram as a map
    for(const auto& pair : fileVector)
    {
        localMap.clear();
        istringstream strstr(pair.first);
        while(getline(strstr, line))
        {
            // Canonic form
            if(canonic)
            {
                unsigned found = line.rfind("\t");
                localMap[line.substr(found+1)]+=1;
            }
            // Raw form
            else {
                unsigned found = line.find("\t");
                localMap[line.substr(0,found)] +=1;
            }
        }
        vectorOfMap.push_back(make_pair(localMap,pair.second));
    }
    return vectorOfMap;
}

const std::vector<std::pair<std::vector<double>,std::vector<double>>> TextController::createGlobalVector(const std::vector<std::pair<std::map<std::string, int>, double> > &globalMap)
{
    map<string, int> templateMap;
    for(const auto& pair : globalMap)
    {
        templateMap.insert(pair.first.begin(), pair.first.end());
    }
    resetToZero(templateMap);

    vector<pair<vector<double>,vector<double>>> globalVector;
    vector<double> localVector;

    for(auto map : globalMap)
    {
        map.first.insert(templateMap.begin(), templateMap.end());
        for(auto& i : map.first) localVector.push_back(static_cast<double>(i.second));
        globalVector.push_back(make_pair(vector<double>(localVector),vector<double>({map.second})));
        resetToZero(templateMap);
        localVector.clear();
    }
    return globalVector;
}

void TextController::resetToZero(std::map<string, int>& map)
{
    for(auto& i : map) i.second=0;
}
