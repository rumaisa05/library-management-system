#ifndef AUDITLOGGER_H
#define AUDITLOGGER_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <ctime>

enum class AuditAction {
    LOGIN,
    LOGOUT,
    VIEW_MEMBERS,
    VIEW_RESOURCES,
    PROCESS_BORROW,
    PROCESS_RETURN,
    ADD_MEMBER,
    DELETE_MEMBER,
    SUSPEND_MEMBER,
    REINSTATE_MEMBER,
    EDIT_MEMBER,
    ADD_ADMIN,
    DELETE_ADMIN,
    VIEW_AUDIT_LOG,
    GENERATE_REPORT,
    DISPLAY_PROFILE,
    CHANGE_PASSWORD
};

struct AuditEntry {
    int entryID;
    int adminID;
    std::string adminUsername;
    AuditAction action;
    std::string targetUser;  // Affected user (if any)
    std::string details;
    std::string timestamp;
    bool success;
};

class AuditLogger {
private:
    std::vector<AuditEntry> entries_;
    std::string filename_;
    int nextEntryID_;
    
    std::string actionToString(AuditAction action) const;
    AuditAction stringToAction(const std::string& str) const;
    std::string boolToString(bool value) const;
    bool stringToBool(const std::string& str) const;
    
public:
    AuditLogger(const std::string& filename = "audit_log.txt");
    ~AuditLogger();
    
    // Log an admin action
    void logAction(int adminID, const std::string& adminUsername, AuditAction action,
                   const std::string& targetUser = "", const std::string& details = "", bool success = true);
    
    // Get all entries
    std::vector<AuditEntry> getAllEntries() const;
    
    // Get entries for a specific admin
    std::vector<AuditEntry> getEntriesByAdmin(int adminID) const;
    
    // Get entries for a specific target user
    std::vector<AuditEntry> getEntriesByTarget(const std::string& targetUser) const;
    
    // Get recent entries
    std::vector<AuditEntry> getRecentEntries(int count) const;
    
    // Search entries by action type
    std::vector<AuditEntry> getEntriesByAction(AuditAction action) const;
    
    // Search entries by date range
    std::vector<AuditEntry> getEntriesByDateRange(const std::string& startDate, const std::string& endDate) const;
    
    // Display audit entries
    void displayEntries(const std::vector<AuditEntry>& entries) const;
    
    // Display all entries
    void displayAllEntries() const;
    
    // Display entries for a specific admin
    void displayEntriesByAdmin(int adminID) const;
    
    // Load from file
    void loadFromFile();
    
    // Save to file
    void saveToFile() const;
    
    // Get entry count
    int getEntryCount() const;
    
    // Clear old entries (older than specified days)
    void clearOldEntries(int daysToKeep);
};

#endif
