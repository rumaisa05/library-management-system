#ifndef BORROWEXTENSION_H
#define BORROWEXTENSION_H

#include <string>
#include <vector>

enum class ExtensionStatus {
    PENDING = 0,
    APPROVED = 1,
    DENIED = 2,
    EXPIRED = 3
};

struct BorrowExtension {
    int extensionID;
    int userID;
    int resourceID;
    std::string originalDueDate;
    std::string newDueDate;
    ExtensionStatus status;
    std::string requestDate;
    std::string responseDate;
    std::string adminNotes;
    int daysExtended;

    BorrowExtension(int id, int uID, int rID, const std::string& origDue,
                   const std::string& reqDate, int days)
        : extensionID(id), userID(uID), resourceID(rID), 
          originalDueDate(origDue), requestDate(reqDate),
          status(ExtensionStatus::PENDING), daysExtended(days),
          responseDate(""), adminNotes(""), newDueDate("") {}

    std::string getStatusName() const;
    std::string serialize() const;
};

class ExtensionManager {
private:
    int resourceID_;
    std::vector<BorrowExtension*> extensions_;
    static const int MAX_EXTENSIONS_PER_BORROW = 2;
    static const int DEFAULT_EXTENSION_DAYS = 7;

public:
    ExtensionManager(int resourceID);
    ~ExtensionManager();

    // Extension requests
    bool canRequestExtension(int userID, int resourceID);
    int requestExtension(int userID, int resourceID, const std::string& currentDueDate,
                        int daysToExtend = DEFAULT_EXTENSION_DAYS);
    
    // Admin actions
    void approveExtension(int extensionID, const std::string& newDueDate);
    void denyExtension(int extensionID, const std::string& reason);
    
    // Queries
    std::vector<BorrowExtension*> getPendingExtensions() const;
    BorrowExtension* findExtension(int extensionID);
    int getExtensionCountForBorrow(int userID, int resourceID) const;

    // Display
    void displayPendingExtensions() const;
    void displayExtensionHistory(int userID, int resourceID) const;

    // Serialization
    std::string serialize() const;
    void deserialize(const std::string& line);

private:
    int generateExtensionID() const;
    std::string addDaysToDate(const std::string& date, int days) const;
};

#endif
