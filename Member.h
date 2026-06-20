#ifndef MEMBER_H
#define MEMBER_H

#include "User.h"
#include <vector>
#include "Notification.h"

struct BorrowedBook {
    int resourceID;
    std::string issueDate;
    std::string dueDate;
    bool isOverdue;
    double lateFee;
};

// Membership types with different privileges
enum class MembershipType {
    SILVER = 0,    // 2 books, 14 days
    GOLD = 1,      // 5 books, 21 days
    PLATINUM = 2   // 10 books, 30 days
};

class Member : public User {
private:
    int borrowedCount_;
    MembershipType membershipType_;
    std::string membershipExpiry_;
    std::vector<BorrowedBook> borrowedBooks_;
    std::vector<Notification*> notifications_;
    int failedLoginAttempts_;
    bool isLocked_;
    std::string lockoutEndTime_;

public:
    Member(int id, std::string username, std::string password,
           std::string first, std::string last, std::string email,
           std::string cnic, std::string phone, std::string address,
           double balance);

    // Inheritance fix - ensure constructors work
    Member(int id, std::string username, std::string password,
           std::string first, std::string last, std::string email,
           std::string cnic, std::string phone, std::string address,
           double balance, bool active);

    // Membership methods
    void setMembershipType(MembershipType type);
    MembershipType getMembershipType() const;
    std::string getMembershipTypeName() const;
    void setMembershipExpiry(const std::string& date);
    std::string getMembershipExpiry() const;
    bool isMembershipValid() const;
    void extendMembership(int months);

    // Borrow limits based on membership
    int getMaxBorrowLimit() const;
    int getBorrowDays() const;

    bool canBorrow() const;
    void incrementBorrow();
    void decrementBorrow();
    void resetBorrowCount();

    // Borrowed book tracking
    void addBorrowedBook(int resourceID, const std::string& issueDate, const std::string& dueDate);
    void removeBorrowedBook(int resourceID);
    void removeAllBorrowedBooks();
    std::vector<BorrowedBook> getBorrowedBooks() const;
    BorrowedBook* findBorrowedBook(int resourceID);
    int getBorrowedCount() const;

    // Late fee calculation
    double calculateLateFee(int resourceID) const;
    double getTotalLateFees() const;
    void updateOverdueStatus();

    // Notification methods
    void addNotification(Notification* notif);
    std::vector<Notification*> getNotifications() const;
    int getUnreadNotificationCount() const;
    void markNotificationAsRead(int notificationID);
    void clearAllNotifications();

// Account security
    void incrementFailedLogin();
    void resetFailedLogin();
    int getFailedLoginAttempts() const;
    void lockAccount(int minutes);
    void unlockAccount();
    bool isLocked() const;
    std::string getLockoutEndTime() const;
    void setBorrowedCount(int count);
    void setFailedLoginAttempts(int attempts);

    void displayProfile() const override;
    void displayFullProfile() const;  // Full profile for admin
    std::string serialize() const override;
    bool login(const std::string& username, const std::string& password) const override;

    static Member* deserialize(const std::string& line);
};

#endif
