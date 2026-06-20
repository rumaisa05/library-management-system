#include "BorrowExtension.h"
#include <iostream>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <iomanip>

using namespace std;

// BorrowExtension implementation
string BorrowExtension::getStatusName() const {
    switch (status) {
        case ExtensionStatus::PENDING: return "Pending Approval";
        case ExtensionStatus::APPROVED: return "Approved";
        case ExtensionStatus::DENIED: return "Denied";
        case ExtensionStatus::EXPIRED: return "Expired";
        default: return "Unknown";
    }
}

string BorrowExtension::serialize() const {
    stringstream ss;
    ss << extensionID << "|" << userID << "|" << resourceID << "|"
       << originalDueDate << "|" << newDueDate << "|" << static_cast<int>(status) << "|"
       << requestDate << "|" << responseDate << "|" << daysExtended << "|"
       << adminNotes;
    return ss.str();
}

// ExtensionManager implementation
ExtensionManager::ExtensionManager(int resourceID)
    : resourceID_(resourceID) {}

ExtensionManager::~ExtensionManager() {
    for (auto ext : extensions_) {
        delete ext;
    }
    extensions_.clear();
}

int ExtensionManager::generateExtensionID() const {
    int maxID = 0;
    for (const auto* ext : extensions_) {
        if (ext->extensionID > maxID)
            maxID = ext->extensionID;
    }
    return maxID + 1;
}

string ExtensionManager::addDaysToDate(const string& date, int days) const {
    struct tm timeinfo = {};
    istringstream iss(date);
    iss >> get_time(&timeinfo, "%Y-%m-%d");
    timeinfo.tm_mday += days;
    mktime(&timeinfo);
    
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", &timeinfo);
    return string(buffer);
}

bool ExtensionManager::canRequestExtension(int userID, int resourceID) {
    int extensionCount = getExtensionCountForBorrow(userID, resourceID);
    return extensionCount < MAX_EXTENSIONS_PER_BORROW;
}

int ExtensionManager::requestExtension(int userID, int resourceID,
                                       const string& currentDueDate,
                                       int daysToExtend) {
    if (!canRequestExtension(userID, resourceID)) {
        throw runtime_error("Maximum extensions reached for this borrow.");
    }

    if (daysToExtend < 1 || daysToExtend > 14) {
        throw runtime_error("Extension days must be between 1 and 14.");
    }

    int extensionID = generateExtensionID();
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    char dateStr[11];
    strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", timeinfo);

    auto ext = new BorrowExtension(extensionID, userID, resourceID,
                                  currentDueDate, dateStr, daysToExtend);
    extensions_.push_back(ext);
    return extensionID;
}

void ExtensionManager::approveExtension(int extensionID, const string& newDueDate) {
    for (auto* ext : extensions_) {
        if (ext->extensionID == extensionID) {
            ext->status = ExtensionStatus::APPROVED;
            ext->newDueDate = newDueDate;
            time_t now = time(nullptr);
            struct tm* timeinfo = localtime(&now);
            char dateStr[11];
            strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", timeinfo);
            ext->responseDate = dateStr;
            break;
        }
    }
}

void ExtensionManager::denyExtension(int extensionID, const string& reason) {
    for (auto* ext : extensions_) {
        if (ext->extensionID == extensionID) {
            ext->status = ExtensionStatus::DENIED;
            ext->adminNotes = reason;
            time_t now = time(nullptr);
            struct tm* timeinfo = localtime(&now);
            char dateStr[11];
            strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", timeinfo);
            ext->responseDate = dateStr;
            break;
        }
    }
}

vector<BorrowExtension*> ExtensionManager::getPendingExtensions() const {
    vector<BorrowExtension*> pending;
    for (auto* ext : extensions_) {
        if (ext->status == ExtensionStatus::PENDING) {
            pending.push_back(ext);
        }
    }
    return pending;
}

BorrowExtension* ExtensionManager::findExtension(int extensionID) {
    for (auto* ext : extensions_) {
        if (ext->extensionID == extensionID) {
            return ext;
        }
    }
    return nullptr;
}

int ExtensionManager::getExtensionCountForBorrow(int userID, int resourceID) const {
    int count = 0;
    for (const auto* ext : extensions_) {
        if (ext->userID == userID && ext->resourceID == resourceID &&
            ext->status != ExtensionStatus::DENIED) {
            count++;
        }
    }
    return count;
}

void ExtensionManager::displayPendingExtensions() const {
    auto pending = getPendingExtensions();
    if (pending.empty()) {
        cout << "No pending extension requests.\n";
        return;
    }

    cout << "\n┌─────────────────────────────────┐\n";
    cout << "│    PENDING EXTENSIONS              │\n";
    cout << "└─────────────────────────────────┘\n";
    for (size_t i = 0; i < pending.size(); i++) {
        cout << "\n[" << (i + 1) << "] Extension ID: " << pending[i]->extensionID << "\n";
        cout << "  User ID: " << pending[i]->userID << "\n";
        cout << "  Resource ID: " << pending[i]->resourceID << "\n";
        cout << "  Current Due Date: " << pending[i]->originalDueDate << "\n";
        cout << "  Requested Extension: " << pending[i]->daysExtended << " days\n";
        cout << "  Request Date: " << pending[i]->requestDate << "\n";
    }
}

void ExtensionManager::displayExtensionHistory(int userID, int resourceID) const {
    cout << "\n┌─────────────────────────────────┐\n";
    cout << "│    EXTENSION HISTORY               │\n";
    cout << "└─────────────────────────────────┘\n";
    bool found = false;
    for (const auto* ext : extensions_) {
        if (ext->userID == userID && ext->resourceID == resourceID) {
            cout << "\nExtension ID: " << ext->extensionID << "\n";
            cout << "  Status: " << ext->getStatusName() << "\n";
            cout << "  Original Due: " << ext->originalDueDate << "\n";
            cout << "  New Due Date: " << ext->newDueDate << "\n";
            cout << "  Days Extended: " << ext->daysExtended << "\n";
            cout << "  Request Date: " << ext->requestDate << "\n";
            if (!ext->responseDate.empty()) {
                cout << "  Response Date: " << ext->responseDate << "\n";
            }
            if (!ext->adminNotes.empty()) {
                cout << "  Notes: " << ext->adminNotes << "\n";
            }
            found = true;
        }
    }
    if (!found) {
        cout << "No extension requests for this borrow.\n";
    }
}

string ExtensionManager::serialize() const {
    stringstream ss;
    ss << resourceID_ << ":" << extensions_.size();
    for (const auto* ext : extensions_) {
        ss << "|" << ext->serialize();
    }
    return ss.str();
}

void ExtensionManager::deserialize(const string& line) {
    stringstream ss(line);
    string token;
    getline(ss, token, ':');  // resourceID
    getline(ss, token, ':');  // count
    
    string extLine;
    while (getline(ss, extLine, '|')) {
        if (!extLine.empty()) {
            stringstream extStream(extLine);
            string extToken;
            
            int extID, userID, resID, status, daysExt;
            string origDue, newDue, reqDate, respDate, notes;
            
            getline(extStream, extToken, '|'); extID = stoi(extToken);
            getline(extStream, extToken, '|'); userID = stoi(extToken);
            getline(extStream, extToken, '|'); resID = stoi(extToken);
            getline(extStream, origDue, '|');
            getline(extStream, newDue, '|');
            getline(extStream, extToken, '|'); status = stoi(extToken);
            getline(extStream, reqDate, '|');
            getline(extStream, respDate, '|');
            getline(extStream, extToken, '|'); daysExt = stoi(extToken);
            getline(extStream, notes, '|');

            auto ext = new BorrowExtension(extID, userID, resID, origDue, reqDate, daysExt);
            ext->newDueDate = newDue;
            ext->status = static_cast<ExtensionStatus>(status);
            ext->responseDate = respDate;
            ext->adminNotes = notes;
            extensions_.push_back(ext);
        }
    }
}
