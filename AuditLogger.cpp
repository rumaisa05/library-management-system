#include "AuditLogger.h"
#include "SecurityUtils.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

string AuditLogger::actionToString(AuditAction action) const {
    switch (action) {
        case AuditAction::LOGIN: return "LOGIN";
        case AuditAction::LOGOUT: return "LOGOUT";
        case AuditAction::VIEW_MEMBERS: return "VIEW_MEMBERS";
        case AuditAction::VIEW_RESOURCES: return "VIEW_RESOURCES";
        case AuditAction::PROCESS_BORROW: return "PROCESS_BORROW";
        case AuditAction::PROCESS_RETURN: return "PROCESS_RETURN";
        case AuditAction::ADD_MEMBER: return "ADD_MEMBER";
        case AuditAction::DELETE_MEMBER: return "DELETE_MEMBER";
        case AuditAction::SUSPEND_MEMBER: return "SUSPEND_MEMBER";
        case AuditAction::REINSTATE_MEMBER: return "REINSTATE_MEMBER";
        case AuditAction::EDIT_MEMBER: return "EDIT_MEMBER";
        case AuditAction::ADD_ADMIN: return "ADD_ADMIN";
        case AuditAction::DELETE_ADMIN: return "DELETE_ADMIN";
        case AuditAction::VIEW_AUDIT_LOG: return "VIEW_AUDIT_LOG";
        case AuditAction::GENERATE_REPORT: return "GENERATE_REPORT";
        case AuditAction::DISPLAY_PROFILE: return "DISPLAY_PROFILE";
        case AuditAction::CHANGE_PASSWORD: return "CHANGE_PASSWORD";
        default: return "UNKNOWN";
    }
}

AuditAction AuditLogger::stringToAction(const string& str) const {
    if (str == "LOGIN") return AuditAction::LOGIN;
    if (str == "LOGOUT") return AuditAction::LOGOUT;
    if (str == "VIEW_MEMBERS") return AuditAction::VIEW_MEMBERS;
    if (str == "VIEW_RESOURCES") return AuditAction::VIEW_RESOURCES;
    if (str == "PROCESS_BORROW") return AuditAction::PROCESS_BORROW;
    if (str == "PROCESS_RETURN") return AuditAction::PROCESS_RETURN;
    if (str == "ADD_MEMBER") return AuditAction::ADD_MEMBER;
    if (str == "DELETE_MEMBER") return AuditAction::DELETE_MEMBER;
    if (str == "SUSPEND_MEMBER") return AuditAction::SUSPEND_MEMBER;
    if (str == "REINSTATE_MEMBER") return AuditAction::REINSTATE_MEMBER;
    if (str == "EDIT_MEMBER") return AuditAction::EDIT_MEMBER;
    if (str == "ADD_ADMIN") return AuditAction::ADD_ADMIN;
    if (str == "DELETE_ADMIN") return AuditAction::DELETE_ADMIN;
    if (str == "VIEW_AUDIT_LOG") return AuditAction::VIEW_AUDIT_LOG;
    if (str == "GENERATE_REPORT") return AuditAction::GENERATE_REPORT;
    if (str == "DISPLAY_PROFILE") return AuditAction::DISPLAY_PROFILE;
    if (str == "CHANGE_PASSWORD") return AuditAction::CHANGE_PASSWORD;
    return AuditAction::LOGIN;
}

string AuditLogger::boolToString(bool value) const {
    return value ? "SUCCESS" : "FAILURE";
}

bool AuditLogger::stringToBool(const string& str) const {
    return str == "SUCCESS";
}

AuditLogger::AuditLogger(const string& filename)
    : filename_(filename), nextEntryID_(1) {
    loadFromFile();
}

AuditLogger::~AuditLogger() {
    saveToFile();
}

void AuditLogger::logAction(int adminID, const string& adminUsername, AuditAction action,
                           const string& targetUser, const string& details, bool success) {
    AuditEntry entry;
    entry.entryID = nextEntryID_++;
    entry.adminID = adminID;
    entry.adminUsername = adminUsername;
    entry.action = action;
    entry.targetUser = targetUser;
    entry.details = details;
    entry.timestamp = SecurityUtils::getCurrentTimestamp();
    entry.success = success;
    
    entries_.push_back(entry);
    saveToFile();
}

vector<AuditEntry> AuditLogger::getAllEntries() const {
    return entries_;
}

vector<AuditEntry> AuditLogger::getEntriesByAdmin(int adminID) const {
    vector<AuditEntry> result;
    for (const auto& entry : entries_) {
        if (entry.adminID == adminID) {
            result.push_back(entry);
        }
    }
    return result;
}

vector<AuditEntry> AuditLogger::getEntriesByTarget(const string& targetUser) const {
    vector<AuditEntry> result;
    for (const auto& entry : entries_) {
        if (entry.targetUser == targetUser) {
            result.push_back(entry);
        }
    }
    return result;
}

vector<AuditEntry> AuditLogger::getRecentEntries(int count) const {
    vector<AuditEntry> result = entries_;
    int size = static_cast<int>(result.size());
    if (count > size) count = size;
    
    vector<AuditEntry> recent;
    for (int i = size - count; i < size; ++i) {
        recent.push_back(result[i]);
    }
    return recent;
}

vector<AuditEntry> AuditLogger::getEntriesByAction(AuditAction action) const {
    vector<AuditEntry> result;
    for (const auto& entry : entries_) {
        if (entry.action == action) {
            result.push_back(entry);
        }
    }
    return result;
}

vector<AuditEntry> AuditLogger::getEntriesByDateRange(const string& startDate, const string& endDate) const {
    vector<AuditEntry> result;
    for (const auto& entry : entries_) {
        if (entry.timestamp >= startDate && entry.timestamp <= endDate) {
            result.push_back(entry);
        }
    }
    return result;
}

void AuditLogger::displayEntries(const vector<AuditEntry>& entries) const {
    if (entries.empty()) {
        cout << "No audit entries found.\n";
        return;
    }
    
    cout << "\n╔═════════════════════════════════┐\n";
    cout << "║    AUDIT LOG ENTRIES              ║\n";
    cout << "╚═════════════════════════════════╝\n";
    for (const auto& entry : entries) {
        cout << "\n[" << entry.entryID << "] " << entry.timestamp << "\n";
        cout << "Admin       : " << entry.adminUsername << " (ID: " << entry.adminID << ")\n";
        cout << "Action     : " << actionToString(entry.action) << "\n";
        if (!entry.targetUser.empty()) {
            cout << "Target     : " << entry.targetUser << "\n";
        }
        if (!entry.details.empty()) {
            cout << "Details    : " << entry.details << "\n";
        }
        cout << "Status     : " << (entry.success ? "SUCCESS" : "FAILURE") << "\n";
        cout << "----------------------\n";
    }
}

void AuditLogger::displayAllEntries() const {
    displayEntries(entries_);
}

void AuditLogger::displayEntriesByAdmin(int adminID) const {
    displayEntries(getEntriesByAdmin(adminID));
}

void AuditLogger::loadFromFile() {
    ifstream file(filename_);
    if (!file.is_open()) {
        nextEntryID_ = 1;
        return;
    }
    
    entries_.clear();
    string line;
    int maxID = 0;
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        istringstream iss(line);
        string token;
        AuditEntry entry;
        
        try {
            getline(iss, token, '|'); entry.entryID = stoi(token);
            getline(iss, token, '|'); entry.adminID = stoi(token);
            getline(iss, entry.adminUsername, '|');
            getline(iss, token, '|'); entry.action = stringToAction(token);
            getline(iss, entry.targetUser, '|');
            getline(iss, entry.details, '|');
            getline(iss, entry.timestamp, '|');
            getline(iss, token, '|'); entry.success = stringToBool(token);
            
            entries_.push_back(entry);
            if (entry.entryID > maxID) maxID = entry.entryID;
        }
        catch (...) {
            // Skip malformed entries
        }
    }
    
    nextEntryID_ = maxID + 1;
}

void AuditLogger::saveToFile() const {
    ofstream file(filename_);
    if (!file.is_open()) return;
    
    for (const auto& entry : entries_) {
        file << entry.entryID << "|";
        file << entry.adminID << "|";
        file << entry.adminUsername << "|";
        file << actionToString(entry.action) << "|";
        file << entry.targetUser << "|";
        file << entry.details << "|";
        file << entry.timestamp << "|";
        file << boolToString(entry.success) << "\n";
    }
}

int AuditLogger::getEntryCount() const {
    return static_cast<int>(entries_.size());
}

void AuditLogger::clearOldEntries(int daysToKeep) {
    // Simple implementation - keep only recent entries
    int keepCount = daysToKeep * 100; // Approximate entries per day
    if (static_cast<int>(entries_.size()) > keepCount) {
        vector<AuditEntry> newEntries;
        int size = static_cast<int>(entries_.size());
        for (int i = size - keepCount; i < size; ++i) {
            newEntries.push_back(entries_[i]);
        }
        entries_ = newEntries;
        saveToFile();
    }
}
