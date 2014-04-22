#ifndef TEXT_CONTROLLER_H
#define TEXT_CONTROLLER_H

#include <string>
#include <vector>
#include <map>

/**
 * @brief The TextController class handle file imporation and histogram creation
 */
class TextController
{
public:
    TextController(const std::string& dataFolderPath);

    /**
     * @brief addSubFolder  Adds a sub folder to the path
     * @param folderName    The subfolder name like 'pos' 'neg'
     */
    void addSubFolder(const std::string& folderName);

    /**
     * @brief addTag    Adds a tag to filter lines in tagged files
     * @param tag       The tag like 'PRE' 'VER'
     */
    void addTag(const std::string& tag);

    /**
     * @brief getGlobalVector creates and returns the global vector
     * @return the global vector
     */
    const std::vector<std::pair<std::vector<double>,std::vector<double>>> getGlobalVector(const bool &canonic=true, const bool &verbose=true);

    /**
     * @brief getGlobalVector creates the global vector from the global map. Public for testing
     * @return the global vector
     */
    const std::vector<std::pair<std::vector<double>,std::vector<double>>> createGlobalVector(const std::vector<std::pair<std::map<std::string, int>, double> > &globalMap);

private:

    /**
     * @brief importFiles Uses getFiles methode on each subFolder
     * @return a vector of pair of string and double
     */
    const std::vector<std::pair<std::string, double>> importFiles();

    /**
     * @brief getFiles Gets the files and sets the file vecor with files content
     * @param files The vector to set
     * @param prefix The subfolder to use
     */
    void getFiles(std::vector<std::pair<std::string, double> > &files, const std::string& prefix);

    /**
     * @brief readFile Reads a file and put it into a simple string line by line
     * @param path The path to the file to read
     * @return The file as a string
     */
    const std::string readFile(const std::string& path);

    /**
     * @brief createGlobalMap Creates an histogram for each file
     * @param fileVector The vector of files
     * @return A vector with the histogram and the associate file score
     */
    const std::vector<std::pair<std::map<std::string, int>,double>> createGlobalMap(const std::vector<std::pair<std::string,double>>& fileVector, const bool& canonic);

    /**
     * @brief keepLine Decide whether or not to keep the line based on tag filter
     * @param line The line to evaluate
     * @return The boolean decision
     */
    bool keepLine(const std::string& line);

    /**
     * @brief resetToZero Put all value to zero in the given map
     * @param map The map to process
     */
    void resetToZero(std::map<std::string, int>& map);

    /**
     * @brief dataFolderPath The main data folder
     */
    const std::string dataFolderPath;

    /**
     * @brief dataFilesIndex The name of the files index file
     */
    const std::string dataFilesIndex;

    /**
     * @brief dataSubFolder The list of subfolder in the main data folder
     */
    std::vector<std::string> dataSubFolder;

    /**
     * @brief tagList The list of tag to keep in the tagged files
     */
    std::vector<std::string> tagList;

    /**
     * @brief posFolder The positif recomandation folder and associate score
     */
    static const std::pair<std::string, double> posFolder;

    /**
     * @brief negFolder The negatif recomandation folder and associate score
     */
    static const std::pair<std::string, double> negFolder;
};

#endif // TEXT_CONTROLLER_H
