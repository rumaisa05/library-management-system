#include "Member.h"
#include "Validator.h"
#include "SecurityUtils.h"
#include <iostream>
#include <sstream>
#include <ctime>
#include <chrono>   // For date manipulation
#include <iomanip>      // For put_time in date formatting
#include <algorithm>

using namespace std;

// Helper to get current date as string
string getCurrentDate() {
    time_t now = time(nullptr);
    tm* ltm = localtime(&now);
    char buffer[11];
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", 
             1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday);
    return string(buffer);
}

// Helper to add days to a date
string addDaysToDate(const string& date, int days) {
    int year, month, day;
    sscanf(date.c_str(), "%d-%d-%d", &year, &month, &day);
    
    tm t = {};
    t.tm_year = year - 1900;
    t.tm_mon = month - 1;
    t.tm_mday = day;
    t.tm_hour = 12;
    
    time_t newTime = mktime(&t) + (days * 24 * 60 * 60);
    tm* newTm = localtime(&newTime);
    
    char buffer[11];
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", 
             1900 + newTm->tm_year, 1 + newTm->tm_mon, newTm->tm_mday);
    return string(buffer);
}

// Helper to check if date is overdue
bool isDateOverdue(const string& dueDate) {
    string current = getCurrentDate();
    return current > dueDate;
}

Member::Member(int id, string username, string password,
               string first, string last, string email,
               string cnic, string phone, string address,
               double balance)
    : User(id, username, password, first, last, email, cnic, phone, address, balance)
{
    borrowedCount_ = 0;
    role_ = "Member";
    membershipType_ = MembershipType::SILVER;
    membershipExpiry_ = addDaysToDate(getCurrentDate(), 365); // 1 year default
    failedLoginAttempts_ = 0;
    isLocked_ = false;
}

Member::Member(int id, string username, string password,
               string first, string last, string email,
               string cnic, string phone, string address,
               double balance, bool active)
    : User(id, username, password, first, last, email, cnic, phone, address, balance)
{
    borrowedCount_ = 0;
    role_ = "Member";
    membershipType_ = MembershipType::SILVER;
    membershipExpiry_ = addDaysToDate(getCurrentDate(), 365);
    failedLoginAttempts_ = 0;
    isLocked_ = false;
    if (!active) {
        suspend();
    }
}

bool Member::login(const string& username, const string& password) const {
    if (isLocked())
        return false;

    return User::login(username, password);
}

// ---------------- MEMBERSHIP METHODS ----------------

void Member::setMembershipType(MembershipType type) {
    membershipType_ = type;
}

MembershipType Member::getMembershipType() const {
    return membershipType_;
}

string Member::getMembershipTypeName() const {
    switch (membershipType_) {
        case MembershipType::SILVER: return "Silver";
        case MembershipType::GOLD: return "Gold";
        case MembershipType::PLATINUM: return "Platinum";
        default: return "Unknown";
    }
}

void Member::setMembershipExpiry(const string& date) {
    membershipExpiry_ = date;
}

string Member::getMembershipExpiry() const {
    return membershipExpiry_;
}

bool Member::isMembershipValid() const {
    return getCurrentDate() <= membershipExpiry_;
}

void Member::extendMembership(int months) {
    int year, month, day;
    string baseDate = membershipExpiry_;
    if (baseDate < getCurrentDate())
        baseDate = getCurrentDate();

    sscanf(baseDate.c_str(), "%d-%d-%d", &year, &month, &day);
    
    month += months;
    while (month > 12) {
        month -= 12;
        year++;
    }
    
    char buffer[11];
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", year, month, day);
    membershipExpiry_ = buffer;
}

int Member::getMaxBorrowLimit() const {
    switch (membershipType_) {
        case MembershipType::SILVER: return 2;
        case MembershipType::GOLD: return 5;
        case MembershipType::PLATINUM: return 10;
        default: return 2;
    }
}

int Member::getBorrowDays() const {
    switch (membershipType_) {
        case MembershipType::SILVER: return 14;
        case MembershipType::GOLD: return 21;
        case MembershipType::PLATINUM: return 30;
        default: return 14;
    }
}

bool Member::canBorrow() const {
    return borrowedCount_ < getMaxBorrowLimit() && isActive() && isMembershipValid();
}

void Member::incrementBorrow() {
    borrowedCount_++;
}

void Member::decrementBorrow() {
    if (borrowedCount_ > 0)
        borrowedCount_--;
}

void Member::resetBorrowCount() {
    borrowedCount_ = 0;
}

void Member::addBorrowedBook(int resourceID, const string& issueDate, const string& dueDate) {
    for (auto& book : borrowedBooks_) {
        if (book.resourceID == resourceID) {
            book.issueDate = issueDate;
            book.dueDate = dueDate;
            book.isOverdue = isDateOverdue(dueDate);
            book.lateFee = 0.0;
            return;
        }
    }

    BorrowedBook book;
    book.resourceID = resourceID;
    book.issueDate = issueDate;
    book.dueDate = dueDate;
    book.isOverdue = isDateOverdue(dueDate);
    book.lateFee = 0.0;
    borrowedBooks_.push_back(book);
}

void Member::removeBorrowedBook(int resourceID) {
    for (auto it = borrowedBooks_.begin(); it != borrowedBooks_.end(); ++it) {
        if (it->resourceID == resourceID) {
            borrowedBooks_.erase(it);
            return;
        }
    }
}

void Member::removeAllBorrowedBooks() {
    borrowedBooks_.clear();
}

vector<BorrowedBook> Member::getBorrowedBooks() const {
    return borrowedBooks_;
}

BorrowedBook* Member::findBorrowedBook(int resourceID) {
    for (auto& book : borrowedBooks_) {
        if (book.resourceID == resourceID)
            return &book;
    }
    return nullptr;
}

int Member::getBorrowedCount() const {
    return borrowedCount_;
}

// ---------------- LATE FEE METHODS ----------------

double Member::calculateLateFee(int resourceID) const {
    for (const auto& book : borrowedBooks_) {
        if (book.resourceID == resourceID) {
            int year, month, day;
            sscanf(book.dueDate.c_str(), "%d-%d-%d", &year, &month, &day);
            
            tm due = {};
            due.tm_year = year - 1900;
            due.tm_mon = month - 1;
            due.tm_mday = day;
            
            time_t dueTime = mktime(&due);
            time_t now = time(nullptr);
            double daysOverdue = difftime(now, dueTime) / (24 * 60 * 60);
            
            return max(0.0, daysOverdue * 10.0);
        }
    }
    return 0.0;
}

double Member::getTotalLateFees() const {
    double total = 0.0;
    for (const auto& book : borrowedBooks_) {
        int year, month, day;
        sscanf(book.dueDate.c_str(), "%d-%d-%d", &year, &month, &day);
        
        tm due = {};
        due.tm_year = year - 1900;
        due.tm_mon = month - 1;
        due.tm_mday = day;
        
        time_t dueTime = mktime(&due);
        time_t now = time(nullptr);
        double daysOverdue = difftime(now, dueTime) / (24 * 60 * 60);
        
        total += max(0.0, daysOverdue * 10.0);
    }
    return total;
}

void Member::updateOverdueStatus() {
    for (auto& book : borrowedBooks_) {
        book.isOverdue = isDateOverdue(book.dueDate);
    }
}

// ---------------- ACCOUNT SECURITY METHODS ----------------

void Member::incrementFailedLogin() {
    failedLoginAttempts_++;
    if (failedLoginAttempts_ >= 5) {
        lockAccount(30); // Lock for 30 minutes after 5 failed attempts
    }
}

void Member::resetFailedLogin() {
    failedLoginAttempts_ = 0;
}

int Member::getFailedLoginAttempts() const {
    return failedLoginAttempts_;
}

void Member::lockAccount(int minutes) {
    isLocked_ = true;
    
    time_t lockEnd = time(nullptr) + (minutes * 60);
    tm* ltm = localtime(&lockEnd);
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d", 
             1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday,
             ltm->tm_hour, ltm->tm_min);
    lockoutEndTime_ = buffer;
}

void Member::unlockAccount() {
    isLocked_ = false;
    failedLoginAttempts_ = 0;
    lockoutEndTime_ = "";
}

bool Member::isLocked() const {
    if (!isLocked_) return false;
    
    // Check if lockout has expired
    if (lockoutEndTime_.empty()) return true;
    
    int year, month, day, hour, minute;
    sscanf(lockoutEndTime_.c_str(), "%d-%d-%d %d:%d", &year, &month, &day, &hour, &minute);
    
    tm lockTime = {};
    lockTime.tm_year = year - 1900;
    lockTime.tm_mon = month - 1;
    lockTime.tm_mday = day;
    lockTime.tm_hour = hour;
    lockTime.tm_min = minute;
    
if (time(nullptr) > mktime(&lockTime)) {
        // Cannot modify member in const method - return false indicates lock has expired
        return false;
    }
    return true;
}

string Member::getLockoutEndTime() const {
    return lockoutEndTime_;
}

void Member::addNotification(Notification* notif) {
    if (notif) {
        notifications_.push_back(notif);
    }
}

std::vector<Notification*> Member::getNotifications() const {
    return notifications_;
}

int Member::getUnreadNotificationCount() const {
    int count = 0;
    for (const auto& notif : notifications_) {
        if (!notif->isRead())
            count++;
    }
    return count;
}

void Member::markNotificationAsRead(int notificationID) {
    for (auto& notif : notifications_) {
        if (notif->getNotificationID() == notificationID) {
            notif->setRead(true);
            return;
        }
    }
}

void Member::clearAllNotifications() {
    for (auto& notif : notifications_) {
        delete notif;
    }
    notifications_.clear();
}

void Member::displayProfile() const {
    cout << "\n=========== MEMBER PROFILE ===========\n";
    cout << "User ID          : " << userID_ << '\n';
    cout << "Role             : " << role_ << '\n';
    cout << "Username         : " << username_ << '\n';
    cout << "Full Name        : " << firstName_ << ' ' << lastName_ << '\n';
    cout << "Email            : " << email_ << '\n';
    cout << "CNIC             : " << cnic_ << '\n';
    cout << "Phone            : " << phoneNumber_ << '\n';
    cout << "Address          : " << address_ << '\n';
    cout << "Membership Type  : " << getMembershipTypeName() << '\n';
    cout << "Membership Expiry: " << membershipExpiry_ << '\n';
    cout << "Borrowed Items   : " << borrowedCount_ << " / " << getMaxBorrowLimit() << '\n';
    if (balance_ > 0) {
        cout << "Account Credit   : Rs. " << balance_ << '\n';
    } else if (balance_ < 0) {
        cout << "Outstanding Fines: Rs. " << -balance_ << '\n';
    } else {
        cout << "Account Balance  : Rs. 0 (No credit, No fines)\n";
    }
    if (isLocked_) {
        cout << "Account Status   : LOCKED until " << lockoutEndTime_ << '\n';
    }
}

void Member::displayFullProfile() const {
    cout << "\n=========== FULL MEMBER PROFILE ===========\n";
    cout << "User ID          : " << userID_ << '\n';
    cout << "Role             : " << role_ << '\n';
    cout << "Username         : " << username_ << '\n';
    cout << "Full Name        : " << firstName_ << ' ' << lastName_ << '\n';
    cout << "Email            : " << email_ << '\n';
    cout << "CNIC             : " << cnic_ << '\n';
    cout << "Phone            : " << phoneNumber_ << '\n';
    cout << "Address          : " << address_ << '\n';
    cout << "Membership Type  : " << getMembershipTypeName() << '\n';
    cout << "Membership Expiry: " << membershipExpiry_ << '\n';
    cout << "Account Status   : " << (isActive() ? "Active" : "Suspended") << '\n';
    if (isLocked_) {
        cout << "Locked Until    : " << lockoutEndTime_ << '\n';
    }
    cout << "Borrowed Items   : " << borrowedCount_ << " / " << getMaxBorrowLimit() << '\n';
    if (balance_ > 0) {
        cout << "Account Credit   : Rs. " << balance_ << '\n';
    } else if (balance_ < 0) {
        cout << "Outstanding Fines: Rs. " << -balance_ << '\n';
    } else {
        cout << "Account Balance  : Rs. 0\n";
    }
    cout << "Unread Notifications : " << getUnreadNotificationCount() << '\n';
    cout << "Failed Login Attempts: " << failedLoginAttempts_ << '\n';
    
    if (!borrowedBooks_.empty()) {
        cout << "\n--- Borrowed Books ---\n";
        for (const auto& book : borrowedBooks_) {
            cout << "Resource ID: " << book.resourceID 
                 << " | Issue: " << book.issueDate 
                 << " | Due: " << book.dueDate;
            if (book.isOverdue) {
                cout << " [OVERDUE]";
            }
            cout << "\n";
        }
    }
}

string Member::serialize() const {
    string activeStr = isActive() ? "ACTIVE" : "SUSPENDED";
    string membType = to_string(static_cast<int>(membershipType_));
    string lockedStr = isLocked_ ? "LOCKED" : "ACTIVE";
    
    // Serialize in format: Member|id|username|password|first|last|email|cnic|phone|address|balance|active|membType|expiry|borrowedCount|locked|lockoutEndTime|failedAttempts
    
    stringstream ss;
    ss << "Member|" << userID_ << "|" << username_ << "|" 
       << password_ << "|" << firstName_ << "|" << lastName_ << "|" 
       << email_ << "|" << cnic_ << "|" << phoneNumber_ << "|" 
       << address_ << "|" << balance_ << "|" << activeStr << "|" 
       << membType << "|" << membershipExpiry_ << "|"
       << borrowedCount_ << "|" << lockedStr << "|" << lockoutEndTime_ << "|"
       << failedLoginAttempts_;
    
    // Add borrowed books as a separate section (format: resourceID~issueDate~dueDate~isOverdue)
    if (!borrowedBooks_.empty()) {
        ss << "|BORROWED:";
        for (size_t i = 0; i < borrowedBooks_.size(); i++) {
            if (i > 0) ss << "~";
            const auto& bb = borrowedBooks_[i];
            ss << bb.resourceID << "," << bb.issueDate << "," << bb.dueDate 
               << "," << (bb.isOverdue ? "1" : "0") << "," << bb.lateFee;
        }
    }
    
    return ss.str();
}

Member* Member::deserialize(const string& line) {
    stringstream ss(line);
    string token;

    getline(ss, token, '|'); // Member

    int id;
    string username, password, first, last, email, cnic, phone, address;
    double balance;
    string activeStr = "ACTIVE";
    string membTypeStr = "0";
    string expiry = "";
    int borrowedCount = 0;
    string lockedStr = "ACTIVE";
    string lockoutTime = "";
    int failedAttempts = 0;
    string borrowedSection = "";

    getline(ss, token, '|'); id = stoi(token);
    getline(ss, username, '|');
    getline(ss, password, '|');
    getline(ss, first, '|');
    getline(ss, last, '|');
    getline(ss, email, '|');
    getline(ss, cnic, '|');
    getline(ss, phone, '|');
    getline(ss, address, '|');
    getline(ss, token, '|'); balance = stod(token);
    
    // Try to read active status (for backwards compatibility)
    if (!ss.eof()) {
        getline(ss, activeStr, '|');
    }
    if (!ss.eof()) {
        getline(ss, membTypeStr, '|');
    }
    if (!ss.eof()) {
        getline(ss, expiry, '|');
    }
    // New fields - try to read borrowedCount (v2 format)
    if (!ss.eof()) {
        getline(ss, token, '|');
        if (!token.empty() && token != "BORROWED:") {
            borrowedCount = stoi(token);
        }
    }
    if (!ss.eof()) {
        getline(ss, lockedStr, '|');
    }
    if (!ss.eof()) {
        getline(ss, lockoutTime, '|');
    }
    if (!ss.eof()) {
        getline(ss, token, '|');
        if (!token.empty() && token != "BORROWED:") {
            failedAttempts = stoi(token);
        }
    }
    // Check for borrowed books section
    if (!ss.eof()) {
        getline(ss, borrowedSection, '|');
    }

    bool active = (activeStr == "ACTIVE");
    bool locked = (lockedStr == "LOCKED");
    
    Member* member = new Member(id, username, password, first, last,
                      email, cnic, phone, address, balance, active);
    
    // Set membership type if provided
    if (!membTypeStr.empty()) {
        member->setMembershipType(static_cast<MembershipType>(stoi(membTypeStr)));
    }
    if (!expiry.empty()) {
        member->setMembershipExpiry(expiry);
    }
    
// Set security fields (new v2 format)
    if (borrowedCount > 0) {
        member->setBorrowedCount(borrowedCount);
    }
    if (locked) {
        member->isLocked_ = true;
        member->lockoutEndTime_ = lockoutTime;
    }
    if (failedAttempts > 0) {
        member->setFailedLoginAttempts(failedAttempts);
    }

    if (borrowedSection.find("BORROWED:") == 0) {
        string items = borrowedSection.substr(9);
        stringstream borrowedStream(items);
        string item;

        while (getline(borrowedStream, item, '~')) {
            if (item.empty())
                continue;

            stringstream itemStream(item);
            string idStr;
            string issueDate;
            string dueDate;
            string overdueStr;
            string feeStr;

            getline(itemStream, idStr, ',');
            getline(itemStream, issueDate, ',');
            getline(itemStream, dueDate, ',');
            getline(itemStream, overdueStr, ',');
            getline(itemStream, feeStr, ',');

            try {
                BorrowedBook book;
                book.resourceID = stoi(idStr);
                book.issueDate = issueDate;
                book.dueDate = dueDate;
                book.isOverdue = (overdueStr == "1") || isDateOverdue(dueDate);
                book.lateFee = feeStr.empty() ? 0.0 : stod(feeStr);
                member->borrowedBooks_.push_back(book);
            }
            catch (...) {
                // Skip malformed borrowed-book entries and keep loading the member.
            }
        }

        if (!member->borrowedBooks_.empty())
            member->borrowedCount_ = static_cast<int>(member->borrowedBooks_.size());
    }
    
    return member;
}

// Setters for deserialization
void Member::setBorrowedCount(int count) {
    borrowedCount_ = count;
}

void Member::setFailedLoginAttempts(int attempts) {
    failedLoginAttempts_ = attempts;
}
