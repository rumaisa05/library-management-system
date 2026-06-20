#include "AdvancedSearch.h"
#include "Resource.h"
#include "Book.h"
#include <algorithm>
#include <cctype>
#include <iostream>

using namespace std;

bool AdvancedSearch::stringContains(const string& haystack, const string& needle, bool caseSensitive) {
    if (needle.empty()) return true;
    
    string h = haystack;
    string n = needle;
    
    if (!caseSensitive) {
        transform(h.begin(), h.end(), h.begin(), ::tolower);
        transform(n.begin(), n.end(), n.begin(), ::tolower);
    }
    
    return h.find(n) != string::npos;
}

vector<Resource*> AdvancedSearch::searchByTitle(const vector<Resource*>& resources,
                                               const string& title) {
    vector<Resource*> results;
    for (auto* res : resources) {
        if (res && stringContains(res->getTitle(), title, false) && res->checkAvailability()) {
            results.push_back(res);
        }
    }
    return results;
}

vector<Resource*> AdvancedSearch::searchByAuthor(const vector<Resource*>& resources,
                                                const string& author) {
    vector<Resource*> results;
    for (auto* res : resources) {
        if (res && stringContains(res->getAuthor(), author, false) && res->checkAvailability()) {
            results.push_back(res);
        }
    }
    return results;
}

vector<Resource*> AdvancedSearch::searchByCategory(const vector<Resource*>& resources,
                                                  const string& category) {
    vector<Resource*> results;
    for (auto* res : resources) {
        if (res && stringContains(res->getCategory(), category, false) && res->checkAvailability()) {
            results.push_back(res);
        }
    }
    return results;
}

vector<Resource*> AdvancedSearch::searchByISBN(const vector<Resource*>& resources,
                                             const string& isbn) {
    vector<Resource*> results;
    for (auto* res : resources) {
        if (res) {
            Book* book = dynamic_cast<Book*>(res);
            if (book && stringContains(book->getISBN(), isbn, true) && res->checkAvailability()) {
                results.push_back(res);
            }
        }
    }
    return results;
}

vector<Resource*> AdvancedSearch::searchByRating(const vector<Resource*>& resources,
                                               double minRating, double maxRating) {
    vector<Resource*> results;
    for (auto* res : resources) {
        if (res) {
            // Rating info would be stored in Resource or Book
            // For now, we'll leave this for future integration
            results.push_back(res);
        }
    }
    return results;
}

vector<Resource*> AdvancedSearch::searchAvailable(const vector<Resource*>& resources) {
    vector<Resource*> results;
    for (auto* res : resources) {
        if (res && res->checkAvailability()) {
            results.push_back(res);
        }
    }
    return results;
}

vector<Resource*> AdvancedSearch::searchUnavailable(const vector<Resource*>& resources) {
    vector<Resource*> results;
    for (auto* res : resources) {
        if (res && !res->checkAvailability()) {
            results.push_back(res);
        }
    }
    return results;
}

bool AdvancedSearch::matchesFilter(Resource* resource, const SearchFilter& filter) {
    if (!resource) return false;
    
    // Title filter
    if (!filter.title.empty()) {
        if (!stringContains(resource->getTitle(), filter.title, false)) {
            return false;
        }
    }
    
    // Author filter
    if (!filter.author.empty()) {
        if (!stringContains(resource->getAuthor(), filter.author, false)) {
            return false;
        }
    }
    
    // Category filter
    if (!filter.category.empty()) {
        if (!stringContains(resource->getCategory(), filter.category, false)) {
            return false;
        }
    }
    
    // ISBN filter
    if (!filter.isbn.empty()) {
        Book* book = dynamic_cast<Book*>(resource);
        if (!book || !stringContains(book->getISBN(), filter.isbn, true)) {
            return false;
        }
    }
    
    // Availability filter
    if (filter.availableOnly && !resource->checkAvailability()) {
        return false;
    }
    
    return true;
}

vector<Resource*> AdvancedSearch::advancedSearch(const vector<Resource*>& resources,
                                                const SearchFilter& filter) {
    vector<Resource*> results;
    for (auto* res : resources) {
        if (matchesFilter(res, filter)) {
            results.push_back(res);
        }
    }
    return results;
}

void AdvancedSearch::sortByTitle(vector<Resource*>& resources, bool ascending) {
    sort(resources.begin(), resources.end(),
        [ascending](Resource* a, Resource* b) {
            int cmp = a->getTitle().compare(b->getTitle());
            return ascending ? cmp < 0 : cmp > 0;
        });
}

void AdvancedSearch::sortByRating(vector<Resource*>& resources, bool descending) {
    sort(resources.begin(), resources.end(),
        [descending](Resource* a, Resource* b) {
            // Rating comparison would go here
            // For now, sort by title as fallback
            int cmp = a->getTitle().compare(b->getTitle());
            return descending ? cmp > 0 : cmp < 0;
        });
}

void AdvancedSearch::sortByPopularity(vector<Resource*>& resources, bool descending) {
    sort(resources.begin(), resources.end(),
        [descending](Resource* a, Resource* b) {
            // Popularity comparison based on borrow count
            // This would require access to borrow statistics
            return false;  // Placeholder
        });
}

map<string, int> AdvancedSearch::getCategoryStats(const vector<Resource*>& resources) {
    map<string, int> stats;
    for (auto* res : resources) {
        if (res) {
            stats[res->getCategory()]++;
        }
    }
    return stats;
}

map<string, int> AdvancedSearch::getAuthorStats(const vector<Resource*>& resources) {
    map<string, int> stats;
    for (auto* res : resources) {
        if (res) {
            stats[res->getAuthor()]++;
        }
    }
    return stats;
}
