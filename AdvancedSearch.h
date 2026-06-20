#ifndef ADVANCEDSEARCH_H
#define ADVANCEDSEARCH_H

#include <string>
#include <vector>
#include <map>

class Resource;  // Forward declaration

struct SearchFilter {
    std::string title;
    std::string author;
    std::string category;
    std::string isbn;
    int minYear;
    int maxYear;
    double minRating;
    double maxRating;
    bool availableOnly;

    SearchFilter() : minYear(0), maxYear(3000), minRating(0.0), 
                    maxRating(5.0), availableOnly(false) {}
};

class AdvancedSearch {
public:
    // Constructor
    AdvancedSearch() = default;

    // Search methods - require external resource collection
    static std::vector<Resource*> searchByTitle(const std::vector<Resource*>& resources,
                                               const std::string& title);
    
    static std::vector<Resource*> searchByAuthor(const std::vector<Resource*>& resources,
                                                const std::string& author);
    
    static std::vector<Resource*> searchByCategory(const std::vector<Resource*>& resources,
                                                  const std::string& category);
    
    static std::vector<Resource*> searchByISBN(const std::vector<Resource*>& resources,
                                             const std::string& isbn);
    
    static std::vector<Resource*> searchByRating(const std::vector<Resource*>& resources,
                                               double minRating, double maxRating);
    
    static std::vector<Resource*> searchAvailable(const std::vector<Resource*>& resources);
    
    static std::vector<Resource*> searchUnavailable(const std::vector<Resource*>& resources);
    
    // Combined advanced search
    static std::vector<Resource*> advancedSearch(const std::vector<Resource*>& resources,
                                                const SearchFilter& filter);
    
    // Sort results
    static void sortByTitle(std::vector<Resource*>& resources, bool ascending = true);
    static void sortByRating(std::vector<Resource*>& resources, bool descending = true);
    static void sortByPopularity(std::vector<Resource*>& resources, bool descending = true);

    // Get filter statistics
    static std::map<std::string, int> getCategoryStats(const std::vector<Resource*>& resources);
    static std::map<std::string, int> getAuthorStats(const std::vector<Resource*>& resources);

private:
    static bool matchesFilter(Resource* resource, const SearchFilter& filter);
    static bool stringContains(const std::string& haystack, const std::string& needle, bool caseSensitive = false);
};

#endif
