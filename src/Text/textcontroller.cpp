#include "include/Text/textcontroller.h"
#include <iostream>
#include <fstream>
#include <QFile>
#include <QTextStream>
#include <stdexcept>

using namespace std;

TextController::TextController(const std::string& dataFolderPath):
    dataFolderPath(dataFolderPath)
{
}

//const std::vector<std::pair<std::vector<double>, std::vector<double>>>& TextController::getGlobalVector()
//{
//    return createGlobalVector(createGlobalMap(importFiles()));
//}

const std::vector<std::string> TextController::importFiles()
{
    vector<string> files;
    getFiles(files, "neg/");
    getFiles(files, "pos/");
    cout << endl << files.size() <<  " files imported" << endl;
    return files;
}

void TextController::getFiles(vector<string>& files, string prefix)
{
    string path = dataFolderPath + prefix + "files.index";
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
}

const std::string TextController::readFile(string path)
{
    QFile file( QString(path.c_str()) );
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw invalid_argument( "Unable to open file" );

    QString line;
    string strFile = "";
    QTextStream in(&file);
    in.setCodec("UTF-8");
    while (!in.atEnd())
    {
        line = in.readLine();
        strFile.append(line.toLocal8Bit().constData());
        strFile.append("\n");
    }
    return strFile;
}


//const std::vector<std::map<std::string, int>>& TextController::createGlobalMap(const std::vector<std::string>& fileVector)
//{
//}

//const std::vector<std::pair<std::vector<double>, std::vector<double>>>& TextController::createGlobalVector(const std::vector<std::hash<std::string, int>>& globalMap)
//{
//}
