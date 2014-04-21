#include "include/Text/textcontroller.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <regex>
#include <sstream>

using namespace std;

TextController::TextController(const std::string& dataFolderPath):
    dataFolderPath(dataFolderPath), dataFilesIndex("files.index")
{
}

const std::vector<std::vector<double>> TextController::getGlobalVector()
{
   vector<string> files = importFiles();
   vector<map<string, int>> map = createGlobalMap(files);
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

const std::vector<std::string> TextController::importFiles()
{
    vector<string> files;
    for (string& folderName : dataSubFolder)
    {
        cout << endl << "Import files from folder " << dataFolderPath + folderName;
        getFiles(files, (folderName + "/") );
    }
    cout << endl << files.size() <<  " files imported" << endl;
    return files;
}

void TextController::getFiles(vector<string>& files, const string &prefix)
{
    string path = dataFolderPath + prefix + dataFilesIndex;
    string line;
    ifstream file (path);
    if (file.is_open())
    {
        while ( getline (file,line) )
        {
            files.push_back(readFile(dataFolderPath + prefix + line));
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


const std::vector<std::map<std::string, int>> TextController::createGlobalMap(const std::vector<std::string>& fileVector)
{
    vector<map<string, int>> vectorOfMap;
    map<string, int> localMap;
    string line;

    for(const string& file : fileVector)
    {
        localMap.clear();
        istringstream strstr(file);
        while(getline(strstr, line))
        {
            unsigned found = line.rfind("\t");
            localMap[line.substr(found+1)]+=1;
        }
        vectorOfMap.push_back(localMap);
    }
    return vectorOfMap;
}

const std::vector<std::vector<double> > TextController::createGlobalVector(const std::vector<std::map<string, int> > &globalMap)
{
    map<string, int> templateMap;
    for(const map<string,int>& map : globalMap)
    {
        templateMap.insert(map.begin(), map.end());
    }
    resetToZero(templateMap);

    vector<vector<double>> globalVector;
    vector<double> localVector;
    cout <<"Starting vector creation (takes a few minutes)" << endl;
    for(map<string,int> map : globalMap)
    {
        map.insert(templateMap.begin(), templateMap.end());
        for(auto& i : map) localVector.push_back(static_cast<double>(i.second));
        globalVector.push_back(vector<double>(localVector));
        resetToZero(templateMap);
        localVector.clear();
    }
    return globalVector;
}

void TextController::resetToZero(std::map<string, int>& map)
{
    for(auto& i : map) i.second=0;
}
