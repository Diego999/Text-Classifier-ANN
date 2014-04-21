#ifndef TEXT_CONTROLLER_H
#define TEXT_CONTROLLER_H

#include <string>
#include <vector>
#include <map>


class TextController
{
public:
    TextController(const std::string& dataFolderPath);

    void addSubFolder(const std::string& folderName);
    void addTag(const std::string& tag);

    const std::vector<std::vector<double>> getGlobalVector();

//private:
    const std::vector<std::string> importFiles();
    void getFiles(std::vector<std::string> &files, const std::string& prefix);
    const std::string readFile(const std::string& path);
    const std::vector<std::map<std::string, int>> createGlobalMap(const std::vector<std::string>& fileVector);
    const std::vector<std::vector<double>> createGlobalVector(const std::vector<std::map<std::string,int>>& globalMap);

    bool keepLine(const std::string& line);
    void resetToZero(std::map<std::string, int>& map);

    const std::string dataFolderPath;
    const std::string dataFilesIndex;
    std::vector<std::string> dataSubFolder;
    std::vector<std::string> tagList;
};

#endif // TEXT_CONTROLLER_H
