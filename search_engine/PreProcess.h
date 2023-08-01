#include <algorithm>
#include <map>
#include <sstream>
#include <vector>

class PreProcess
{
public:
    inline static const std::vector<char> punctuations{
        '{', '}', '[', ']', '<', '>', '=', '(', ')', '.', ',', ';', '\'', '\"', '?', '#', '!', '-', ':'};

    inline static const std::vector<std::string> stopWords{"a",
                                                           "an",
                                                           "the",
                                                           "they",
                                                           "these",
                                                           "this",
                                                           "for",
                                                           "is",
                                                           "are",
                                                           "was",
                                                           "of",
                                                           "or",
                                                           "and",
                                                           "does",
                                                           "will",
                                                           "whose"};

    inline static const std::map<std::string, std::string> pluralToSingular{
        {"stacks", "stack"}, {"structures", "structure"}, {"applications", "application"}};

    static std::string toLower(const std::string& line)
    {
        std::string lowercaseLine = line;
        std::transform(lowercaseLine.begin(), lowercaseLine.end(), lowercaseLine.begin(), ::tolower);
        return lowercaseLine;
    }

    static std::string removePunctuations(const std::string& line)
    {
        std::string result;
        for (char c : line)
        {
            if (std::find(punctuations.begin(), punctuations.end(), c) == punctuations.end())
            {
                result += c;
            }
            else
            {
                result += " ";
            }
        }
        return result;
    }

    static std::string replacePluralsWithSingulars(const std::string& line)
    {
        // Assuming words are separated by spaces
        std::string result;
        size_t      startPos = 0;
        size_t      endPos   = line.find(' ');

        while (endPos != std::string::npos)
        {
            std::string word = line.substr(startPos, endPos - startPos);
            auto        it   = pluralToSingular.find(word);
            if (it != pluralToSingular.end())
            {
                result += it->second + " ";
            }
            else
            {
                result += word + " ";
            }
            startPos = endPos + 1;
            endPos   = line.find(' ', startPos);
        }

        // Process the last word
        std::string lastWord = line.substr(startPos);
        auto        it       = pluralToSingular.find(lastWord);
        if (it != pluralToSingular.end())
        {
            result += it->second;
        }
        else
        {
            result += lastWord;
        }

        return result;
    }

    static std::string preprocess(const std::string& line)
    {
        auto lowercase    = toLower(line);
        auto withoutPunct = removePunctuations(lowercase);
        auto result       = replacePluralsWithSingulars(withoutPunct);
        return result;
    }

    static std::vector<std::string> splitString(const std::string& str, char delimiter)
    {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string       token;
        while (std::getline(ss, token, delimiter))
        {
            tokens.push_back(token);
        }
        return tokens;
    }
};