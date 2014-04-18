#ifndef TEXT_CONTROLLER_H
#define TEXT_CONTROLLER_H

#include <string>
#include <vector>
#include <map>


class TextController
{
public:
    TextController(const std::string& dataFolderPath);

    void addSubFolder(std::string folderName);
    void addTag(std::string tag);

//    const std::vector<std::pair<std::vector<double>, std::vector<double>>>& getGlobalVector();

//private:
    const std::vector<std::string> importFiles();
    void getFiles(std::vector<std::string>& files, std::string prefix);
    const std::string readFile(std::string path);
    const std::vector<std::map<std::string, int>> createGlobalMap(const std::vector<std::string>& fileVector);
//    const std::vector<std::pair<std::vector<double>, std::vector<double>>>& createGlobalVector(const std::vector<std::map<std::string, int>>& globalMap);

    bool keepLine(std::string line);

    const std::string dataFolderPath;
    const std::string dataFilesIndex;
    std::vector<std::string> dataSubFolder;
    std::vector<std::string> tagList;
};

#endif // TEXT_CONTROLLER_H
