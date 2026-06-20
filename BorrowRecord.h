#ifndef BORROWRECORD_H
#define BORROWRECORD_H

#include <string>

class BorrowRecord {
private:
    int recordID_;
    int userID_;
    int resourceID_;

    std::string issueDate_;
    std::string dueDate_;
    std::string returnDate_;

    bool isReturned_;
    bool isDigital_;
    double fineAmount_;

    // helper moved inside class (FIXED DESIGN)
    int convertToDays(const std::string& date) const;

public:
    BorrowRecord();

    void createRecord(int resourceID, int recordID, int userID,
                      std::string issueDate, std::string dueDate,
                      bool isDigital = false);

    void updateReturnDate(std::string date);

    bool isDigital() const;
    double calculateFine();

    void displayRecord() const;

    int getRecordID() const;
    int getUserID() const;
    int getResourceID() const;

    std::string getIssueDate() const;
    std::string getDueDate() const;
    std::string getReturnDate() const;

    std::string getStatus() const;  // Returns dynamic status based on current date

    double getFineAmount() const;
    std::string getCurrentDate() const;
    int getCurrentDateAsDays() const;
    std::string serialize() const;

    static BorrowRecord* deserialize(const std::string& line);
};

#endif
