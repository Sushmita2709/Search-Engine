#include <map>
#include <string>
#include <vector>


class WebPage
{
public:
    WebPage(const std::string& file_path, const std::string& page_name) : name(page_name), filePath(file_path)
    {
        readFileContents();
        processLines();
        buildPageIndex();
    }

    const std::string& getName() const { return name; }

    const std::vector<std::string>& getLines() const { return lines; }

    const std::map<std::string, std::vector<int>>& getPageIndex() const { return pageIndex; }

    void printPageIndex() const;

    double TermFrequency(const std::string& word, bool isPhrase = false) const;

private:
    std::string name;
    std::string filePath;
    std::vector<std::string> lines;
    std::string processedText;
    std::map<std::string, std::vector<int>> pageIndex;

    void readFileContents();
    void processLines();
    void buildPageIndex();
    int countSubstringOccurrences(const std::string& phrase) const;
};