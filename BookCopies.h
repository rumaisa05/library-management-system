#ifndef BOOKCOPIES_H
#define BOOKCOPIES_H

#include <string>
#include <vector>

enum class CopyCondition {
    EXCELLENT = 0,
    GOOD = 1,
    FAIR = 2,
    POOR = 3
};

struct BookCopy {
    int copyID;
    std::string barcode;
    CopyCondition condition;
    bool isAvailable;
    int borrowCount;
    std::string lastMaintenanceDate;
    std::string acquiredDate;

    BookCopy(int id, const std::string& code, CopyCondition cond,
             const std::string& acquired)
        : copyID(id), barcode(code), condition(cond), isAvailable(true),
          borrowCount(0), lastMaintenanceDate(""), acquiredDate(acquired) {}

    std::string getConditionName() const;
    std::string serialize() const;
    static BookCopy deserialize(const std::string& line);
};

class BookCopiesManager {
private:
    int resourceID_;
    std::vector<BookCopy*> copies_;

public:
    BookCopiesManager(int resourceID);
    ~BookCopiesManager();

    // Copy management
    int addCopy(const std::string& barcode, CopyCondition condition);
    void removeCopy(int copyID);
    
    // Availability
    bool hasAvailableCopy() const;
    BookCopy* getAvailableCopy();
    void markCopyBorrowed(int copyID);
    void markCopyReturned(int copyID);
    
    // Statistics
    int getTotalCopies() const { return copies_.size(); }
    int getAvailableCopiesCount() const;
    double getAverageUtilization() const;
    
    // Display
    void displayAllCopies() const;
    std::string serialize() const;
    void deserialize(const std::string& line);

private:
    int generateCopyID() const;
};

// Reading Status for tracking what members are reading
enum class ReadingStatus {
    WANT_TO_READ = 0,
    CURRENTLY_READING = 1,
    COMPLETED = 2
};

struct MemberReadingStatus {
    int resourceID;
    ReadingStatus status;
    std::string startedDate;
    std::string completedDate;
    int pagesRead;

    MemberReadingStatus(int id) : resourceID(id), status(ReadingStatus::WANT_TO_READ),
                                  pagesRead(0), startedDate(""), completedDate("") {}

    std::string getStatusName() const;
    std::string serialize() const;
};

#endif
