#include "WebPage.h"
#include <map>
#include <memory>
#include <utility>

class SearchEngine
{
public:
    SearchEngine() = default;

    void addWebPage(const std::string& filePath, const std::string& pageName);

    const std::map<std::string, std::vector<std::pair<int, int>>>& getInvertedPageIndex() const
    {
        return invertedPageIndex;
    }

    void printInvertedPageIndex() const;
    void findPagesContainingWord(const std::string& word) const;
    void queryFindPositionsOfWordInAPage(const std::string& word, const std::string& pageName) const;
    void printSearchResult(const std::map<int, double>& relevanceMap);
    std::map<int, double> OrQuery(std::vector<std::string> tokens);
    std::map<int, double> AndQuery(std::vector<std::string> tokens);
    std::map<int, double> PhraseQuery(std::vector<std::string> tokens);

private:
    std::map<int, std::shared_ptr<WebPage>> webpages;
    std::map<std::string, std::vector<std::pair<int, int>>> invertedPageIndex;
    int nextId = 1;

    std::shared_ptr<WebPage> getPageById(int pageId) const;
    // Function to calculate the inverse document frequency for a word
    double InverseDocumentFrequency(const std::string& word, bool isPhrase = false) const;
    double calculateRelevance(const std::string& word, int pageId, bool isPhrase = false) const;
    std::map<int, double> getRelevanceMap(const std::string& word, bool isPhrase = false) const;
};