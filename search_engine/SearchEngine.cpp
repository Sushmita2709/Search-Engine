#include "SearchEngine.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <set>
#include <sstream>

// Custom comparator to sort based on values
template <typename K, typename V>
struct ValueComparator
{
    bool operator()(const std::pair<K, V>& lhs, const std::pair<K, V>& rhs) { return lhs.second > rhs.second; }
};


void SearchEngine::addWebPage(const std::string& filePath, const std::string& pageName)
{
    // Create a new WebPage object using std::make_shared
    auto newPage = std::make_shared<WebPage>(filePath, pageName);
    // Add the newPage to the webpages vector
    webpages[nextId++] = newPage;

    // Update the invertedPageIndex with the words from the new page
    const auto& pageIndex = newPage->getPageIndex();
    for (const auto& entry : pageIndex)
    {
        const std::string& word = entry.first;
        const std::vector<int>& occurrences = entry.second;

        for (int index : occurrences)
        {
            invertedPageIndex[word].push_back(std::make_pair(nextId - 1, index));
        }
    }
}

void SearchEngine::printInvertedPageIndex() const
{
    std::cout << "Inverted PageIndex:" << std::endl;
    for (const auto& entry : invertedPageIndex)
    {
        std::cout << entry.first << ": ";
        for (const auto& pair : entry.second)
        {
            std::cout << "(" << pair.first << "," << pair.second << ") ";
        }
        std::cout << std::endl;
    }
}

std::shared_ptr<WebPage> SearchEngine::getPageById(int pageId) const
{
    auto it = webpages.find(pageId);
    if (it != webpages.end())
    {
        return it->second;
    }
    return nullptr;
}

void SearchEngine::findPagesContainingWord(const std::string& word) const
{
    // Get the inverted index
    const auto& invertedIndex = getInvertedPageIndex();

    // Find pages containing the word
    auto it = invertedIndex.find(word);
    if (it != invertedIndex.end())
    {
        std::cout << "Pages containing the word '" << word << "':" << std::endl;
        std::vector<int> distinctPages;

        for (const auto& pair : it->second)
        {
            int pageId = pair.first;
            std::shared_ptr<WebPage> page = getPageById(pageId);
            if (page)
            {
                // Check if the page is already present to avoid duplicates
                if (std::find(distinctPages.begin(), distinctPages.end(), pageId) == distinctPages.end())
                {
                    distinctPages.push_back(pageId);
                }
            }
        }

        // Print distinctPages comma-separated
        if (!distinctPages.empty())
        {
            std::stringstream ss;
            for (size_t i = 0; i < distinctPages.size() - 1; ++i)
            {
                std::shared_ptr<WebPage> page = getPageById(distinctPages[i]);
                ss << page->getName() << ", ";
            }
            ss << getPageById(distinctPages.back())->getName();
            std::cout << ss.str() << std::endl;
        }
    }
    else
    {
        std::cout << "No webpage contains word \'" << word << "\'" << std::endl;
    }
}

void SearchEngine::queryFindPositionsOfWordInAPage(const std::string& word, const std::string& pageName) const
{
    std::shared_ptr<WebPage> page = nullptr;
    for (const auto& entry : webpages)
    {
        if (entry.second->getName() == pageName)
        {
            page = entry.second;
            break;
        }
    }

    if (!page)
    {
        std::cout << "No webpage named " << pageName << " found." << std::endl;
        return;
    }

    const auto& pageIndex = page->getPageIndex();
    auto        itWord    = pageIndex.find(word);

    if (itWord != pageIndex.end())
    {
        std::cout << "Word '" << word << "' found at indices: ";
        const std::vector<int>& occurrences = itWord->second;

        for (size_t i = 0; i < occurrences.size(); ++i)
        {
            std::cout << occurrences[i];
            if (i < occurrences.size() - 1)
            {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;
    }
    else
    {
        std::cout << "Webpage " << pageName << " does not contain word '" << word << "'." << std::endl;
    }
}

double SearchEngine::InverseDocumentFrequency(const std::string& word, bool isPhrase) const
{
    if (not isPhrase)
    {
        auto CountUniquePagesWithWord = [&]() {
            auto it = invertedPageIndex.find(word);
            if (it != invertedPageIndex.end())
            {
                const std::vector<std::pair<int, int>>& occurrences = it->second;
                std::set<int> uniquePageIds;
                for (const auto& occurrence : occurrences)
                {
                    uniquePageIds.insert(occurrence.first);
                }
                return static_cast<int>(uniquePageIds.size());
            }
            return 0;  // Word not found in the invertedPageIndex
        };

        // Calculate the number of pages where the word is present
        int numPagesWithWord = CountUniquePagesWithWord();

        // Get the total number of webpages
        int totalWebpages = webpages.size();

        // Calculate the inverse document frequency
        if (numPagesWithWord > 0)
        {
            double idf = static_cast<double>(totalWebpages) / static_cast<double>(numPagesWithWord);
            return log10(idf);
        }
        else if (totalWebpages == 0)
        {
            return 0.0;
        }
        return 0.0;  // Avoid taking the log of zero if the word is not present in any page
    }
    else
    {
        int numPagesWithPhrase = 0;
        for (auto w : webpages)
        {
            auto x = (w.second)->TermFrequency(word, true);
            if (x != 0)
            {
                numPagesWithPhrase += 1;
            }
        }

        // Get the total number of webpages
        int totalWebpages = webpages.size();

        // Calculate the inverse document frequency
        if (numPagesWithPhrase > 0)
        {
            double idf = static_cast<double>(totalWebpages) / static_cast<double>(numPagesWithPhrase);
            return log10(idf);
        }
        else if (totalWebpages == 0)
        {
            return 0.0;
        }
        return 0.0;  // Avoid taking the log of zero if the word is not present in any page
    }
}

double SearchEngine::calculateRelevance(const std::string& word, int pageId, bool isPhrase) const
{
    // Find the WebPage with the given pageId
    auto it = webpages.find(pageId);
    if (it == webpages.end())
    {
        return 0.0;  // PageId not found in the database, so relevance is 0
    }

    std::shared_ptr<WebPage> page = it->second;
    auto tf = page->TermFrequency(word, isPhrase);
    if (tf == 0)
    {
        return 0.0;
    }
    auto idf = InverseDocumentFrequency(word, isPhrase);
    return tf * idf;
}

// Function to get the relevance map for a given word.
// The map contains pageIDs as keys and their relevance scores as values.
std::map<int, double> SearchEngine::getRelevanceMap(const std::string& word, bool isPhrase) const
{
    std::map<int, double> relevanceMap;

    // Calculate relevance for each webpage (pageID) for the given word.
    for (const auto& entry : webpages)
    {
        int    pageId    = entry.first;
        double relevance = calculateRelevance(word, pageId, isPhrase);
        relevanceMap[pageId] = relevance;
    }

    return relevanceMap;
}

// Function to print the search result based on the relevance map.
// The function extracts page names from the relevance map and prints them in sorted order of relevance.
void SearchEngine::printSearchResult(const std::map<int, double>& relevanceMap)
{
    // Create a vector of pairs from the relevance map.
    std::vector<std::pair<int, double>> sortedVector(relevanceMap.begin(), relevanceMap.end());

    // Sort the vector based on values using the custom comparator ValueComparator<int, double>().
    std::sort(sortedVector.begin(), sortedVector.end(), ValueComparator<int, double>());

    std::vector<std::string> names;

    // Extract page names with non-zero relevance and store them in the names vector.
    for (const auto& pair : sortedVector)
    {
        if (pair.second != 0)
        {
            names.push_back(getPageById(pair.first)->getName());
        }
    }

    // Print the names comma-separated.
    for (size_t i = 0; i < names.size() - 1; i++)
    {
        std::cout << names[i] << ", ";
    }
    std::cout << names.back() << std::endl;
}

// Function to perform OR query for a vector of words (tokens).
// The function returns a map containing pageIDs and their combined relevance scores for the OR query.
std::map<int, double> SearchEngine::OrQuery(std::vector<std::string> tokens)
{
    std::cout << "Pages containg ANY of the words (Ranked Descending wrt relevance) : ";
    for (size_t i = 1; i < tokens.size() - 1; i++)
    {
        std::cout << tokens[i] << ", ";
    }
    std::cout << tokens.back() << std::endl;

    std::map<int, double> result;

    // Initialize the result map with all pageIDs and set their relevance scores to 0.
    for (const auto& entry : webpages)
    {
        int pageId = entry.first;
        result[pageId] = 0;
    }

    // Calculate the relevance scores for each word and update the result map with the combined scores (OR).
    for (size_t i = 1; i < tokens.size(); i++)
    {
        auto tempMap = getRelevanceMap(tokens[i]);
        for (const auto& vals : tempMap)
        {
            result[vals.first] += vals.second;
        }
    }

    return result;
}

// Function to perform AND query for a vector of words (tokens).
// The function returns a map containing pageIDs and their combined relevance scores for the AND query.
std::map<int, double> SearchEngine::AndQuery(std::vector<std::string> tokens)
{
    std::cout << "Pages containg ALL of the words (Ranked Descending wrt relevance) : ";
    for (size_t i = 1; i < tokens.size() - 1; i++)
    {
        std::cout << tokens[i] << ", ";
    }
    std::cout << tokens.back() << std::endl;

    std::map<int, double> result;

    // Initialize the result map with all pageIDs and set their relevance scores to 1.
    for (const auto& entry : webpages)
    {
        int pageId = entry.first;
        result[pageId] = 1;
    }

    // Calculate the relevance scores for each word and update the result map with the combined scores (AND).
    for (size_t i = 1; i < tokens.size(); i++)
    {
        auto tempMap = getRelevanceMap(tokens[i]);
        for (const auto& vals : tempMap)
        {
            result[vals.first] *= vals.second;
        }
    }

    return result;
}

// Function to perform Phrase query for a vector of words (tokens).
// The function returns a map containing pageIDs and their combined relevance scores for the AND query.
std::map<int, double> SearchEngine::PhraseQuery(std::vector<std::string> tokens)
{
    std::cout << "Pages containing the Phrase (Ranked Descending wrt relevance) : ";
    std::string phrase = " ";
    for (size_t i = 1; i < tokens.size() - 1; i++)
    {
        phrase += tokens[i] + " ";
    }
    phrase += tokens.back();
    std::cout << phrase << std::endl;

    auto result = getRelevanceMap(phrase, true);
    return result;
}