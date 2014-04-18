#include "include/Text/textcontroller.h"
#include <iostream>
#include <fstream>
#include <QFile>
#include <QTextStream>
#include <stdexcept>
#include <regex>

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

void TextController::addSubFolder(std::string folderName)
{
    dataSubFolder.push_back(folderName);
}

void TextController::addTag(std::string tag)
{
    tagList.push_back(tag);
}

const std::vector<std::string> TextController::importFiles()
{
    vector<string> files;
    for (string folderName : dataSubFolder)
    {
        cout << endl << "Import files from folder " << dataFolderPath + folderName;
        getFiles(files, (folderName + "/") );
    }
    cout << endl << files.size() <<  " files imported" << endl;
    return files;
}

void TextController::getFiles(vector<string>& files, string prefix)
{
    string path = dataFolderPath + prefix + dataFilesIndex;
    QFile file( QString(path.c_str()) );
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw invalid_argument( "Unable to open file" );

    QString line;
    QTextStream in(&file);
    in.setCodec("UTF-8");
    while (!in.atEnd())
    {
        line = in.readLine();
        files.push_back(readFile(dataFolderPath + prefix + line.toLocal8Bit().constData()));
    }
    file.close();
}

const std::string TextController::readFile(string path)
{
    QFile file( QString(path.c_str()) );
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw invalid_argument( "Unable to open file" );

    QString line;
    string strLine;
    string strFile = "";
    QTextStream in(&file);
    in.setCodec("UTF-8");
    while (!in.atEnd())
    {
        line = in.readLine();
        strLine = line.toLocal8Bit().constData();
        if (keepLine(strLine))
        {
            strFile.append(strLine);
            strFile.append("\n");
        }
    }
    file.close();
    return strFile;
}

 bool TextController::keepLine(std::string line)
 {
     bool flag = false;
     for (string tag : tagList)
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


const std::vector<std::map<std::string, int>> TextController::createGlobalMap(const std::vector<std::string> fileVector)
{
    vector<map<string, int>> vectorOfMap;
    map<string, int> localMap;
    string line;

    for(string file : fileVector)
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

const std::vector<std::vector<double> > TextController::createGlobalVector(std::vector<std::map<string, int> >& globalMap)
{
    map<string, int> templateMap;
    for(map<string,int> map : globalMap)
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
