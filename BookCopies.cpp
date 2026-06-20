#include "BookCopies.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>

using namespace std;

// BookCopy implementation
string BookCopy::getConditionName() const {
    switch (condition) {
        case CopyCondition::EXCELLENT: return "Excellent";
        case CopyCondition::GOOD: return "Good";
        case CopyCondition::FAIR: return "Fair";
        case CopyCondition::POOR: return "Poor - Needs Maintenance";
        default: return "Unknown";
    }
}

string BookCopy::serialize() const {
    stringstream ss;
    ss << copyID << "," << barcode << "," << static_cast<int>(condition) << ","
       << isAvailable << "," << borrowCount << "," << lastMaintenanceDate << ","
       << acquiredDate;
    return ss.str();
}

BookCopy BookCopy::deserialize(const string& line) {
    stringstream ss(line);
    string token;
    int id, cond, available, count;
    string barcode, maintenance, acquired;

    getline(ss, token, ','); id = stoi(token);
    getline(ss, token, ','); barcode = token;
    getline(ss, token, ','); cond = stoi(token);
    getline(ss, token, ','); available = stoi(token);
    getline(ss, token, ','); count = stoi(token);
    getline(ss, token, ','); maintenance = token;
    getline(ss, token, ','); acquired = token;

    BookCopy copy(id, barcode, static_cast<CopyCondition>(cond), acquired);
    copy.isAvailable = available;
    copy.borrowCount = count;
    copy.lastMaintenanceDate = maintenance;
    return copy;
}

// BookCopiesManager implementation
BookCopiesManager::BookCopiesManager(int resourceID)
    : resourceID_(resourceID) {}

BookCopiesManager::~BookCopiesManager() {
    for (auto copy : copies_) {
        delete copy;
    }
    copies_.clear();
}

int BookCopiesManager::generateCopyID() const {
    int maxID = 0;
    for (const auto* copy : copies_) {
        if (copy->copyID > maxID)
            maxID = copy->copyID;
    }
    return maxID + 1;
}

int BookCopiesManager::addCopy(const string& barcode, CopyCondition condition) {
    int copyID = generateCopyID();
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    char dateStr[11];
    strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", timeinfo);

    auto copy = new BookCopy(copyID, barcode, condition, dateStr);
    copies_.push_back(copy);
    return copyID;
}

void BookCopiesManager::removeCopy(int copyID) {
    auto it = find_if(copies_.begin(), copies_.end(),
                     [copyID](BookCopy* c) { return c->copyID == copyID; });
    if (it != copies_.end()) {
        delete *it;
        copies_.erase(it);
    }
}

bool BookCopiesManager::hasAvailableCopy() const {
    for (const auto* copy : copies_) {
        if (copy->isAvailable && copy->condition != CopyCondition::POOR) {
            return true;
        }
    }
    return false;
}

BookCopy* BookCopiesManager::getAvailableCopy() {
    for (auto* copy : copies_) {
        if (copy->isAvailable && copy->condition != CopyCondition::POOR) {
            return copy;
        }
    }
    return nullptr;
}

void BookCopiesManager::markCopyBorrowed(int copyID) {
    for (auto* copy : copies_) {
        if (copy->copyID == copyID) {
            copy->isAvailable = false;
            copy->borrowCount++;
            break;
        }
    }
}

void BookCopiesManager::markCopyReturned(int copyID) {
    for (auto* copy : copies_) {
        if (copy->copyID == copyID) {
            copy->isAvailable = true;
            break;
        }
    }
}

int BookCopiesManager::getAvailableCopiesCount() const {
    int count = 0;
    for (const auto* copy : copies_) {
        if (copy->isAvailable && copy->condition != CopyCondition::POOR) {
            count++;
        }
    }
    return count;
}

double BookCopiesManager::getAverageUtilization() const {
    if (copies_.empty()) return 0.0;
    int totalBorrows = 0;
    for (const auto* copy : copies_) {
        totalBorrows += copy->borrowCount;
    }
    return static_cast<double>(totalBorrows) / copies_.size();
}

void BookCopiesManager::displayAllCopies() const {
    cout << "\n┌─────────────────────────────────┐\n";
    cout << "│    AVAILABLE BOOK COPIES (" << copies_.size() << " total)       │\n";
    cout << "└─────────────────────────────────┘\n";
    for (size_t i = 0; i < copies_.size(); i++) {
        cout << "\n[" << (i + 1) << "] Copy ID: " << copies_[i]->copyID << "\n";
        cout << "  Barcode: " << copies_[i]->barcode << "\n";
        cout << "  Status: " << (copies_[i]->isAvailable ? "Available" : "Borrowed") << "\n";
        cout << "  Condition: " << copies_[i]->getConditionName() << "\n";
        cout << "  Times Borrowed: " << copies_[i]->borrowCount << "\n";
        cout << "  Acquired: " << copies_[i]->acquiredDate << "\n";
    }
    cout << "\nAvailable Copies: " << getAvailableCopiesCount() << "/" << copies_.size() << "\n";
}

string BookCopiesManager::serialize() const {
    stringstream ss;
    ss << resourceID_ << ":" << copies_.size();
    for (const auto* copy : copies_) {
        ss << "|" << copy->serialize();
    }
    return ss.str();
}

void BookCopiesManager::deserialize(const string& line) {
    stringstream ss(line);
    string token;
    getline(ss, token, ':');  // resourceID
    getline(ss, token, ':');  // count
    
    string copyLine;
    while (getline(ss, copyLine, '|')) {
        if (!copyLine.empty()) {
            BookCopy copy = BookCopy::deserialize(copyLine);
            copies_.push_back(new BookCopy(copy));
        }
    }
}

// MemberReadingStatus implementation
string MemberReadingStatus::getStatusName() const {
    switch (status) {
        case ReadingStatus::WANT_TO_READ: return "Want to Read";
        case ReadingStatus::CURRENTLY_READING: return "Currently Reading";
        case ReadingStatus::COMPLETED: return "Completed";
        default: return "Unknown";
    }
}

string MemberReadingStatus::serialize() const {
    stringstream ss;
    ss << resourceID << "," << static_cast<int>(status) << ","
       << startedDate << "," << completedDate << "," << pagesRead;
    return ss.str();
}
