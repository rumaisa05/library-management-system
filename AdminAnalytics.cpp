#include "AdminAnalytics.h"
#include "LibraryManager.h"
#include "UserManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>

using namespace std;

AdminAnalytics::AdminAnalytics(LibraryManager* lm, UserManager* um)
    : libManager_(lm), userManager_(um) {}

LibraryStats AdminAnalytics::generateLibraryStats() const {
    LibraryStats stats;
    stats.totalMembers = 0;
    stats.activeBorrows = 0;
    stats.totalResources = 0;
    stats.availableResources = 0;
    stats.totalFinesOutstanding = 0.0;
    stats.averageFinePerMember = 0.0;
    stats.overdueBooks = 0;
    stats.libraryUtilization = 0.0;
    
    if (userManager_ && libManager_) {
        stats.totalMembers = userManager_->getMemberCount();
        stats.totalResources = libManager_->getResources().size();
        
        // Count available resources
        stats.availableResources = 0;
        for (const auto* res : libManager_->getResources()) {
            if (res && res->checkAvailability()) {
                stats.availableResources++;
            }
        }
        
        auto activeBorrows = libManager_->getActiveBorrows();
        stats.activeBorrows = activeBorrows.size();
        
        // Count overdue records and calculate total fines
        for (const auto* record : activeBorrows) {
            if (record) {
                string status = record->getStatus();
                if (status == "Overdue") {
                    stats.overdueBooks++;
                }
                stats.totalFinesOutstanding += record->getFineAmount();
            }
        }
        
        if (stats.totalMembers > 0) {
            stats.averageFinePerMember = stats.totalFinesOutstanding / stats.totalMembers;
        }
        
        if (stats.totalResources > 0) {
            stats.libraryUtilization = (static_cast<double>(stats.activeBorrows) / 
                                       stats.totalResources) * 100.0;
        }
    }
    
    return stats;
}

vector<PopularBook> AdminAnalytics::getPopularBooks(int limit) const {
    vector<PopularBook> popularBooks;
    map<int, int> borrowCounts;  // resourceID -> count
    
    if (!libManager_) return popularBooks;
    
    // Count borrows for each resource
    auto activeBorrows = libManager_->getActiveBorrows();
    for (const auto* record : activeBorrows) {
        if (record) {
            borrowCounts[record->getResourceID()]++;
        }
    }
    
    // Get all resources and populate popular books
    for (const auto* res : libManager_->getResources()) {
        if (res) {
            PopularBook pb;
            pb.resourceID = res->getResourceID();
            pb.title = res->getTitle();
            pb.author = res->getAuthor();
            pb.borrowCount = borrowCounts[res->getResourceID()];
            pb.rating = 0.0;
            popularBooks.push_back(pb);
        }
    }
    
    // Sort by borrow count (descending)
    sort(popularBooks.begin(), popularBooks.end(),
        [](const PopularBook& a, const PopularBook& b) {
            return a.borrowCount > b.borrowCount;
        });
    
    // Return top 'limit' items
    if (popularBooks.size() > (size_t)limit) {
        popularBooks.resize(limit);
    }
    
    return popularBooks;
}

map<string, int> AdminAnalytics::getMostBorrowedCategories(int limit) const {
    map<string, int> categoryStats;
    
    if (!libManager_) return categoryStats;
    
    auto activeBorrows = libManager_->getActiveBorrows();
    map<string, int> borrowCounts;
    
    // Count borrows by category
    for (const auto* record : activeBorrows) {
        if (record) {
            Resource* res = libManager_->searchByID(record->getResourceID());
            if (res) {
                borrowCounts[res->getCategory()]++;
            }
        }
    }
    
    // Convert to vector and sort
    vector<pair<string, int>> sorted;
    for (const auto& p : borrowCounts) {
        sorted.push_back(p);
    }
    sort(sorted.begin(), sorted.end(),
        [](const pair<string, int>& a, const pair<string, int>& b) {
            return a.second > b.second;
        });
    
    // Add top items
    int count = 0;
    for (const auto& p : sorted) {
        if (count >= limit) break;
        categoryStats[p.first] = p.second;
        count++;
    }
    
    return categoryStats;
}

map<string, int> AdminAnalytics::getMostBorrowedAuthors(int limit) const {
    map<string, int> authorStats;
    
    if (!libManager_) return authorStats;
    
    auto activeBorrows = libManager_->getActiveBorrows();
    map<string, int> borrowCounts;
    
    // Count borrows by author
    for (const auto* record : activeBorrows) {
        if (record) {
            Resource* res = libManager_->searchByID(record->getResourceID());
            if (res) {
                borrowCounts[res->getAuthor()]++;
            }
        }
    }
    
    // Convert to vector and sort
    vector<pair<string, int>> sorted;
    for (const auto& p : borrowCounts) {
        sorted.push_back(p);
    }
    sort(sorted.begin(), sorted.end(),
        [](const pair<string, int>& a, const pair<string, int>& b) {
            return a.second > b.second;
        });
    
    // Add top items
    int count = 0;
    for (const auto& p : sorted) {
        if (count >= limit) break;
        authorStats[p.first] = p.second;
        count++;
    }
    
    return authorStats;
}

vector<MemberActivity> AdminAnalytics::getActiveMembers(int limit) const {
    vector<MemberActivity> memberActivities;
    
    if (!userManager_) return memberActivities;
    
    const auto& users = userManager_->getUsers();
    for (const auto* user : users) {
        if (user && user->getRole() == "Member") {
            MemberActivity ma;
            ma.userID = user->getUserID();
            ma.username = user->getUsername();
            ma.totalFines = user->getBalance();
            // More data would be populated if we had access to Member-specific methods
            memberActivities.push_back(ma);
        }
    }
    
    // Sort by borrow count (descending)
    sort(memberActivities.begin(), memberActivities.end(),
        [](const MemberActivity& a, const MemberActivity& b) {
            return a.totalBorrows > b.totalBorrows;
        });
    
    if (memberActivities.size() > (size_t)limit) {
        memberActivities.resize(limit);
    }
    
    return memberActivities;
}

vector<MemberActivity> AdminAnalytics::getMembersWithOverdueBooks() const {
    vector<MemberActivity> overdueMembers;
    set<int> userIDsWithOverdue;
    
    if (!libManager_) return overdueMembers;
    
    auto activeBorrows = libManager_->getActiveBorrows();
    for (const auto* record : activeBorrows) {
        if (record) {
            string status = record->getStatus();
            if (status == "Overdue") {
                userIDsWithOverdue.insert(record->getUserID());
            }
        }
    }
    
    // Create member activity entries for members with overdue books
    if (userManager_) {
        const auto& users = userManager_->getUsers();
        for (const auto* user : users) {
            if (user && userIDsWithOverdue.count(user->getUserID())) {
                MemberActivity ma;
                ma.userID = user->getUserID();
                ma.username = user->getUsername();
                ma.totalBorrows = 0;
                ma.activeBorrows = 0;
                ma.totalFines = user->getBalance();
                overdueMembers.push_back(ma);
            }
        }
    }
    
    return overdueMembers;
}

vector<MemberActivity> AdminAnalytics::getHighestFineMembers(int limit) const {
    vector<MemberActivity> fineMembers;
    
    if (!userManager_) return fineMembers;
    
    const auto& users = userManager_->getUsers();
    for (const auto* user : users) {
        if (user && user->getRole() == "Member") {
            MemberActivity ma;
            ma.userID = user->getUserID();
            ma.username = user->getUsername();
            ma.totalFines = user->getBalance();
            if (ma.totalFines < 0) {
                fineMembers.push_back(ma);
            }
        }
    }
    
    // Sort by fines (descending - most negative first)
    sort(fineMembers.begin(), fineMembers.end(),
        [](const MemberActivity& a, const MemberActivity& b) {
            return a.totalFines < b.totalFines;  // Most negative (highest fines) first
        });
    
    if (fineMembers.size() > (size_t)limit) {
        fineMembers.resize(limit);
    }
    
    return fineMembers;
}

int AdminAnalytics::getBorrowsInDateRange(const string& startDate, const string& endDate) const {
    // This would require accessing borrow history and filtering by date
    return 0;  // Placeholder
}

int AdminAnalytics::getReturnsInDateRange(const string& startDate, const string& endDate) const {
    // This would require accessing return history and filtering by date
    return 0;  // Placeholder
}

double AdminAnalytics::getTotalOutstandingFines() const {
    double total = 0.0;
    if (!userManager_) return total;
    
    const auto& users = userManager_->getUsers();
    for (const auto* user : users) {
        if (user && user->getBalance() < 0) {
            total += -user->getBalance();  // Add the absolute value of negative balances
        }
    }
    return total;
}

double AdminAnalytics::getCollectedFinesInDateRange(const string& startDate, const string& endDate) const {
    // This would require tracking fine payment history
    return 0.0;  // Placeholder
}

int AdminAnalytics::getOverdueBookCount() const {
    if (!libManager_) return 0;
    
    int overdueCount = 0;
    auto activeBorrows = libManager_->getActiveBorrows();
    for (const auto* record : activeBorrows) {
        if (record) {
            string status = record->getStatus();
            if (status == "Overdue") {
                overdueCount++;
            }
        }
    }
    
    return overdueCount;
}

double AdminAnalytics::getAverageUtilizationRate() const {
    if (!libManager_) return 0.0;
    auto stats = generateLibraryStats();
    return stats.libraryUtilization;
}

int AdminAnalytics::getResourcesNeedingMaintenance() const {
    // This would check resource condition status
    return 0;  // Placeholder
}

void AdminAnalytics::displayDashboard() const {
    auto stats = generateLibraryStats();
    
    cout << "\n========================================\n";
    cout << "     LIBRARY MANAGEMENT DASHBOARD\n";
    cout << "========================================\n";
    cout << "\n--- QUICK STATISTICS ---\n";
    cout << "Total Members: " << stats.totalMembers << "\n";
    cout << "Total Resources: " << stats.totalResources << "\n";
    cout << "Available Resources: " << stats.availableResources << "\n";
    cout << "Active Borrows: " << stats.activeBorrows << "\n";
    cout << "Library Utilization: " << fixed << setprecision(1) << stats.libraryUtilization << "%\n";
    cout << "Overdue Books: " << stats.overdueBooks << "\n";
    
    cout << "\n--- FINANCIAL SUMMARY ---\n";
    cout << "Total Outstanding Fines: Rs. " << fixed << setprecision(2) << stats.totalFinesOutstanding << "\n";
    cout << "Average Fine per Member: Rs. " << stats.averageFinePerMember << "\n";
    
    cout << "\n--- TOP 5 POPULAR BOOKS ---\n";
    auto popular = getPopularBooks(5);
    for (size_t i = 0; i < popular.size(); i++) {
        cout << (i + 1) << ". " << popular[i].title << " - " << popular[i].borrowCount << " borrows\n";
    }
    
    cout << "\n--- TOP 5 HIGH FINE MEMBERS ---\n";
    auto highFines = getHighestFineMembers(5);
    for (size_t i = 0; i < highFines.size(); i++) {
        cout << (i + 1) << ". " << highFines[i].username << " - Rs. " 
             << fixed << setprecision(2) << -highFines[i].totalFines << "\n";
    }
}

void AdminAnalytics::displayMemberAnalytics() const {
    cout << "\n===================================\n";
    cout << "    MEMBER ANALYTICS\n";
    cout << "===================================\n";
    auto members = getActiveMembers(10);
    cout << "Total Active Members: " << members.size() << "\n";
    
    auto withOverdue = getMembersWithOverdueBooks();
    cout << "Members with Overdue Books: " << withOverdue.size() << "\n";
    
    auto withFines = getHighestFineMembers(5);
    cout << "\nTop 5 Members with Fines:\n";
    for (size_t i = 0; i < withFines.size(); i++) {
        cout << (i + 1) << ". " << withFines[i].username << " - Rs. " 
             << fixed << setprecision(2) << -withFines[i].totalFines << "\n";
    }
}

void AdminAnalytics::displayBookAnalytics() const {
    cout << "\n===================================\n";
    cout << "    BOOK ANALYTICS\n";
    cout << "===================================\n";
    auto popular = getPopularBooks(10);
    cout << "\nTop 10 Popular Books:\n";
    for (size_t i = 0; i < popular.size(); i++) {
        cout << (i + 1) << ". " << popular[i].title << " by " << popular[i].author 
             << " - " << popular[i].borrowCount << " borrows\n";
    }
    
    auto categories = getMostBorrowedCategories(5);
    cout << "\nTop Categories:\n";
    int idx = 1;
    for (const auto& cat : categories) {
        cout << (idx++) << ". " << cat.first << " - " << cat.second << " total borrows\n";
    }
}

void AdminAnalytics::displayFineAnalytics() const {
    cout << "\n===================================\n";
    cout << "    FINE ANALYTICS\n";
    cout << "===================================\n";
    double totalFines = getTotalOutstandingFines();
    int overdueCount = getOverdueBookCount();
    
    cout << "Total Outstanding Fines: Rs. " << fixed << setprecision(2) << totalFines << "\n";
    cout << "Overdue Books: " << overdueCount << "\n";
    
    auto highFines = getHighestFineMembers(10);
    cout << "\nTop 10 Members with Fines:\n";
    for (size_t i = 0; i < highFines.size(); i++) {
        cout << (i + 1) << ". " << highFines[i].username << " - Rs. " 
             << fixed << setprecision(2) << -highFines[i].totalFines << "\n";
    }
}

void AdminAnalytics::displayResourceAnalytics() const {
    cout << "\n┌─────────────────────────────────┐\n";
    cout << "│    RESOURCE ANALYTICS            │\n";
    cout << "└─────────────────────────────────┘\n";
    auto stats = generateLibraryStats();
    
    cout << "Total Resources: " << stats.totalResources << "\n";
    cout << "Available Resources: " << stats.availableResources << "\n";
    cout << "Borrowed Resources: " << (stats.totalResources - stats.availableResources) << "\n";
    cout << "Utilization Rate: " << fixed << setprecision(1) << stats.libraryUtilization << "%\n";
    
    auto categories = getMostBorrowedCategories(5);
    cout << "\nResources by Category:\n";
    int idx = 1;
    for (const auto& cat : categories) {
        cout << (idx++) << ". " << cat.first << " - " << cat.second << " items\n";
    }
}

string AdminAnalytics::generateHTMLReport(const string& filename) const {
    // Placeholder for HTML report generation
    return filename;
}
