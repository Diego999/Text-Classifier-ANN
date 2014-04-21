#include "include/Text/textcontroller.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <regex>
#include <sstream>
#include <utility>

using namespace std;

TextController::TextController(const std::string& dataFolderPath):
    dataFolderPath(dataFolderPath), dataFilesIndex("files.index")
{
}

const std::vector<std::pair<std::vector<double>, std::vector<double> > > TextController::getGlobalVector()
{
   std::vector<std::pair<std::string,double>> files = importFiles();
   vector<pair<map<string, int>,double>> map = createGlobalMap(files);
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
        cout << endl << "Import files from folder " << dataFolderPath + folderName;
        getFiles(files, (folderName + "/") );
    }
    cout << endl << files.size() <<  " files imported" << endl;
    return files;
}

void TextController::getFiles(std::vector<std::pair<string,double>>& files, const string &prefix)
{
    string path = dataFolderPath + prefix + dataFilesIndex;
    string line;
                                                 double score;
    ifstream file (path);
    if (file.is_open())
    {
        if(prefix=="pos/")
            score = 0.9;
        else
            score = 0.1;
        while ( getline (file,line) )
        {
            files.push_back(make_pair(readFile(dataFolderPath + prefix + line),score));
        }
    }
    file.close();
}

const std::string TextController::readFile(const string& path)
{
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
         std::size_t found = line.find(tag);
         if (found!=std::string::npos)
         {
            flag = true;
            break;
         }
     }
     return flag;
 }


const std::vector<std::pair<std::map<std::string, int>,double>> TextController::createGlobalMap(const std::vector<std::pair<std::string,double>>& fileVector)
{
    std::vector<std::pair<std::map<std::string, int>,double>> vectorOfMap;
    map<string, int> localMap;
    string line;

    for(const auto& pair : fileVector)
    {
        localMap.clear();
        istringstream strstr(pair.first);
        while(getline(strstr, line))
        {
            unsigned found = line.rfind("\t");
            localMap[line.substr(found+1)]+=1;
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
    cout <<"Starting vector creation (takes a few minutes)" << endl;
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
