#include "BorrowRecord.h"
#include "SYSTEMEXCEPTIONS.h"
#include "Validator.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <ctime>
#include <iomanip>

using namespace std;

// ---------------- CONSTRUCTOR ----------------

BorrowRecord::BorrowRecord() {
    recordID_ = 0;
    userID_ = 0;
    resourceID_ = 0;

    issueDate_ = "";
    dueDate_ = "";
    returnDate_ = "";

    fineAmount_ = 0;
    isReturned_ = false;
    isDigital_ = false;
}

// ---------------- GETTERS ----------------

int BorrowRecord::getRecordID() const { return recordID_; }
int BorrowRecord::getUserID() const { return userID_; }
int BorrowRecord::getResourceID() const { return resourceID_; }

string BorrowRecord::getIssueDate() const { return issueDate_; }
string BorrowRecord::getDueDate() const { return dueDate_; }
string BorrowRecord::getReturnDate() const { return returnDate_; }

string BorrowRecord::getStatus() const {
    if (isReturned_)
        return "Returned";
    
    // Get current date
    time_t now = time(nullptr);
    tm* currentTm = localtime(&now);
    
    char currentDate[11];
    strftime(currentDate, sizeof(currentDate), "%Y-%m-%d", currentTm);
    
    try {
        int due = convertToDays(dueDate_);
        int today = convertToDays(string(currentDate));
        
        if (today > due)
            return "Overdue";
        else
            return "Borrowed";
    }
    catch (...) {
        return "Borrowed";
    }
}

double BorrowRecord::getFineAmount() const {
    if (isDigital_)
        return 0;

    if (!isReturned_) {
        try {
            int due = convertToDays(dueDate_);
            int today = getCurrentDateAsDays();
            int daysLate = max(0, today - due);
            return daysLate * 10.0;
        }
        catch (...) {
            return 0;
        }
    }
    return fineAmount_;
}

string BorrowRecord::getCurrentDate() const {
    time_t now = time(nullptr);
    tm* currentTm = localtime(&now);
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", currentTm);
    return string(buf);
}

int BorrowRecord::getCurrentDateAsDays() const {
    return convertToDays(getCurrentDate());
}

// ---------------- SAFE DATE CONVERSION ----------------

int BorrowRecord::convertToDays(const string& date) const {
    // Proper day calculation accounting for leap years and varying month lengths
    if (date.length() < 10)
        throw InvalidDateException("Invalid date format (YYYY-MM-DD)");

    int y = stoi(date.substr(0, 4));
    int m = stoi(date.substr(5, 2));
    int d = stoi(date.substr(8, 2));

    if (m < 1 || m > 12 || d < 1 || d > 31)
        throw InvalidDateException("Invalid date values: " + date);

    // Calculate days from year 0 with proper month lengths
    int daysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    
    // Add leap year day for February if applicable
    if (m > 2 && ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)))
        daysInMonth[2] = 29;
    
    // Count days from all complete years
    int totalDays = y * 365 + y / 4 - y / 100 + y / 400;
    
    // Add days from all complete months in current year
    for (int month = 1; month < m; month++) {
        totalDays += daysInMonth[month];
    }
    
    // Add days in current month
    totalDays += d;
    
    return totalDays;
}

// ---------------- CREATE RECORD ----------------

void BorrowRecord::createRecord(int resourceID, int recordID, int userID,
                                string issueDate, string dueDate,
                                bool isDigital) {
    Validator::validateID(userID);
    Validator::validateID(resourceID);
    Validator::validateID(recordID);
    Validator::validateDate(issueDate);
    Validator::validateDate(dueDate);

    if (convertToDays(dueDate) < convertToDays(issueDate))
        throw InvalidDateException("Due date cannot be earlier than issue date");

    userID_ = userID;
    resourceID_ = resourceID;
    recordID_ = recordID;

    issueDate_ = issueDate;
    dueDate_ = dueDate;

    returnDate_ = "";
    fineAmount_ = 0;
    isReturned_ = false;
    isDigital_ = isDigital;
}

// ---------------- RETURN BOOK ----------------

void BorrowRecord::updateReturnDate(string date) {
    if (isReturned_)
        throw AlreadyReturnedException("Book already returned");

    Validator::validateDate(date);

    if (convertToDays(date) < convertToDays(issueDate_))
        throw InvalidDateException("Return date cannot be earlier than issue date");
    
    // Validate return date is not in the future
    if (convertToDays(date) > convertToDays(getCurrentDate()))
        throw InvalidDateException("Return date cannot be in the future");

    returnDate_ = date;
    isReturned_ = true;

    calculateFine();
}

bool BorrowRecord::isDigital() const {
    return isDigital_;
}

// ---------------- FINE CALCULATION ----------------

double BorrowRecord::calculateFine() {
    if (isDigital_) {
        fineAmount_ = 0;
        return 0;
    }

    try {
        int due = convertToDays(dueDate_);
        int endDate = isReturned_ ? convertToDays(returnDate_) : getCurrentDateAsDays();
        int daysLate = max(0, endDate - due);

        // Apply a simple flat daily rate for every day after the due date.
        fineAmount_ = daysLate * 10.0;
        return fineAmount_;
    }
    catch (...) {
        fineAmount_ = 0;
        return 0;
    }
}

// ---------------- DISPLAY (FIXED CONST) ----------------

void BorrowRecord::displayRecord() const {
    cout << "\n╔═════════════════════════════════┐\n";
    cout << "║    BORROW RECORD DETAILS           ║\n";
    cout << "╚═════════════════════════════════╝\n";
    cout << "Record ID   : " << recordID_ << endl;
    cout << "User ID     : " << userID_ << endl;
    cout << "Resource ID : " << resourceID_ << endl;
    cout << "Issue Date  : " << issueDate_ << endl;
    cout << "Due Date    : " << dueDate_ << endl;
    cout << "Return Date : " << (returnDate_.empty() ? "Not returned" : returnDate_) << endl;
    cout << "Status      : " << getStatus() << endl;
    cout << "Fine        : " << getFineAmount() << endl;
}

string BorrowRecord::serialize() const {
    return to_string(recordID_) + "|" +
           to_string(userID_) + "|" +
           to_string(resourceID_) + "|" +
           issueDate_ + "|" +
           dueDate_ + "|" +
           returnDate_ + "|" +
           (isReturned_ ? "1" : "0") + "|" +
           (isDigital_ ? "1" : "0") + "|" +
           to_string(fineAmount_);
}

BorrowRecord* BorrowRecord::deserialize(const string& line) {
    stringstream ss(line);
    string token;

    auto* record = new BorrowRecord();

    getline(ss, token, '|');
    record->recordID_ = stoi(token);

    getline(ss, token, '|');
    record->userID_ = stoi(token);

    getline(ss, token, '|');
    record->resourceID_ = stoi(token);

    getline(ss, record->issueDate_, '|');
    getline(ss, record->dueDate_, '|');
    getline(ss, record->returnDate_, '|');

    getline(ss, token, '|');
    record->isReturned_ = (token == "1");

    string nextToken;
    if (getline(ss, nextToken, '|')) {
        if (ss.good()) {
            record->isDigital_ = (nextToken == "1");
            getline(ss, token, '|');
            record->fineAmount_ = token.empty() ? 0.0 : stod(token);
        } else {
            // Old format: digital flag not present
            record->isDigital_ = false;
            record->fineAmount_ = nextToken.empty() ? 0.0 : stod(nextToken);
        }
    } else {
        record->isDigital_ = false;
        record->fineAmount_ = 0.0;
    }

    Validator::validateID(record->recordID_);
    Validator::validateID(record->userID_);
    Validator::validateID(record->resourceID_);
    Validator::validateDate(record->issueDate_);
    Validator::validateDate(record->dueDate_);

    if (record->isReturned_) {
        Validator::validateDate(record->returnDate_);
        record->calculateFine();
    }
    else {
        record->returnDate_.clear();
        record->fineAmount_ = 0.0;
    }

    return record;
}
