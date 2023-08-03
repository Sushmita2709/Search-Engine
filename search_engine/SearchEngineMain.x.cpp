#include "PreProcess.h"
#include "SearchEngine.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

std::string getFilePathRelativeToExecutable(const std::string& relativePath)
{
    // Get the current source file's path using __FILE__
    std::string sourceFilePath(__FILE__);

    // Remove the source file name to get the directory path
    size_t lastSlashPos = sourceFilePath.find_last_of("/\\");
    if (lastSlashPos != std::string::npos)
    {
        sourceFilePath = sourceFilePath.substr(0, lastSlashPos);
    }

    // Concatenate the relative path with the directory path
    return sourceFilePath + "/" + relativePath;
}

int main()
{
    SearchEngine searchEngine;

    std::string   actionsFilePath = getFilePathRelativeToExecutable("../data/actions.txt");
    std::ifstream actionsFile(actionsFilePath);

    if (!actionsFile.is_open())
    {
        std::cerr << "Error: Could not open actions.txt file." << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(actionsFile, line))
    {
        auto originalLine               = line;
        auto processedLine              = PreProcess::preprocess(line);
        std::vector<std::string> tokens = PreProcess::splitString(originalLine, ' ');
        std::vector<std::string> processedTokens = PreProcess::splitString(processedLine, ' ');

        if (tokens[0] == "addPage" && tokens.size() == 2)
        {
            std::string filename        = tokens[1];
            std::string webpageFilePath = getFilePathRelativeToExecutable("../data/webpages/" + filename);
            searchEngine.addWebPage(webpageFilePath, filename);
            std::cout << "Added webpage: " << filename << std::endl;
        }
        else if (tokens[0] == "queryFindPagesWhichContainWord" && tokens.size() == 2)
        {
            std::string wordToFind = PreProcess::preprocess(tokens[1]);
            searchEngine.findPagesContainingWord(wordToFind);
        }
        else if (tokens[0] == "queryFindPositionsOfWordInAPage" && tokens.size() == 3)
        {
            std::string wordToFind = PreProcess::preprocess(tokens[1]);
            const std::string& pageName = tokens[2];
            searchEngine.queryFindPositionsOfWordInAPage(wordToFind, pageName);
        }
        else if (tokens[0] == "queryFindPagesWhichContainAllWords" && tokens.size() >= 2)
        {
            auto result = searchEngine.AndQuery(processedTokens);
            searchEngine.printSearchResult(result);
        }
        else if (tokens[0] == "queryFindPagesWhichContainAnyOfTheseWords" && tokens.size() >= 2)
        {
            auto result = searchEngine.OrQuery(processedTokens);
            searchEngine.printSearchResult(result);
        }
        else if (tokens[0] == "queryFindPagesWhichContainPhrase" && tokens.size() >= 2)
        {
            auto result = searchEngine.PhraseQuery(processedTokens);
            searchEngine.printSearchResult(result);
        }
        else
        {
            std::cerr << "Error: Unknown command - " << tokens[0] << std::endl;
        }
    }

    actionsFile.close();

    return 0;
}
