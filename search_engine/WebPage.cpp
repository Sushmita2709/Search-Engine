#include "WebPage.h"
#include "PreProcess.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>

void WebPage::readFileContents()
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        throw std::runtime_error("Error: Could not open file - " + filePath);
    }

    std::string line;
    while (std::getline(file, line))
    {
        lines.push_back(line);
    }

    file.close();
}

void WebPage::processLines()
{
    // Construct the processedText by appending all the processed lines
    std::ostringstream oss;
    for (const auto& line : lines)
    {
        // Convert the line to lowercase before processing
        std::string lowercaseLine = PreProcess::toLower(line);
        ;
        std::string processedLine = PreProcess::removePunctuations(lowercaseLine);
        processedLine = PreProcess::replacePluralsWithSingulars(processedLine);

        // Append the processed line to the processedText
        oss << processedLine << " ";
    }
    processedText = oss.str();  // Set the processed text member variable
}

void WebPage::buildPageIndex()
{
    std::istringstream iss(processedText);
    std::string        word;
    int                index = 1;

    auto stopWords = PreProcess::stopWords;
    // Split the processedText into words and build the pageIndex
    while (iss >> word)
    {
        // Check if the word is not a stop word
        if (std::find(stopWords.begin(), stopWords.end(), word) == stopWords.end())
        {
            pageIndex[word].push_back(index);
        }
        index++;
    }
}

void WebPage::printPageIndex() const
{
    std::cout << "PageIndex of " << name << " :" << std::endl;
    for (const auto& entry : pageIndex)
    {
        std::cout << entry.first << ": ";
        for (size_t i = 0; i < entry.second.size(); ++i)
        {
            std::cout << entry.second[i];
            if (i < entry.second.size() - 1)
            {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;
    }
}

int WebPage::countSubstringOccurrences(const std::string& phrase) const
{
    int    count = 0;
    size_t pos   = 0;

    while ((pos = processedText.find(phrase, pos)) != std::string::npos)
    {
        ++count;
        pos += phrase.length();  // Move past the current occurrence
    }

    return count;
}

double WebPage::TermFrequency(const std::string& word, bool isPhrase) const
{
    // Get the total number of words in the processedText
    std::istringstream iss(processedText);
    int totalWords = std::distance(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>());
    if (not isPhrase)
    {
        auto it = pageIndex.find(word);
        if (it != pageIndex.end())
        {

            // Calculate the term frequency of the word
            int occurrences = it->second.size();
            return static_cast<double>(occurrences) / static_cast<double>(totalWords);
        }
        return 0.0;  // Word not found, so return 0.0 frequency
    }
    else
    {
        int k = 1;

        for (char ch : word)
        {
            if (ch == ' ')
            {
                k++;
            }
        }

        auto m    = countSubstringOccurrences(word);
        auto deno = totalWords - (k - 1) * m;
        if (deno != 0)
        {
            return static_cast<double>(m) / static_cast<double>(deno);
        }
        return 0.0;
    }
}