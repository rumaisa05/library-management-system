#include "ReportGenerator.h"
#include "SYSTEMEXCEPTIONS.h"
#include "Validator.h"
#include <sstream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <iostream>

using namespace std;

// ---------------- CONSTRUCTOR ----------------

ReportGenerator::ReportGenerator() {}

// Helper method to get current timestamp
static string getCurrentTimestamp() {
    time_t now = time(nullptr);
    tm* timeinfo = localtime(&now);
    ostringstream ss;
    ss << put_time(timeinfo, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Helper method to escape PDF special characters
static string escapePDFString(const string& str) {
    string result;
    for (char c : str) {
        if (c == '(' || c == ')' || c == '\\') {
            result += '\\';
        }
        result += c;
    }
    return result;
}

// ============ TEXT REPORT GENERATION ============

// -------- USER REPORT --------

string ReportGenerator::generateUserReport(int userID,
                                           const vector<BorrowRecord>& records) const
{
    Validator::validateID(userID);

    ostringstream report;
    report << "===== USER REPORT =====\n";
    report << "Generated on: " << getCurrentTimestamp() << "\n";
    report << "User ID: " << userID << "\n";
    report << "=====================================\n\n";

    bool found = false;

    for (const auto& record : records)
    {
        if (record.getUserID() == userID)
        {
            found = true;

            report << "Record ID   : " << record.getRecordID() << "\n";
            report << "Resource ID : " << record.getResourceID() << "\n";
            report << "Issue Date  : " << record.getIssueDate() << "\n";
            report << "Due Date    : " << record.getDueDate() << "\n";
            report << "Return Date : " << record.getReturnDate() << "\n";
            report << "Fine        : $" << fixed << setprecision(2) << record.getFineAmount() << "\n";
            report << "---------------------------\n";
        }
    }

    if (!found)
        throw EmptyReportException("No borrow records found for this user");

    return report.str();
}

// -------- RESOURCE REPORT --------

string ReportGenerator::generateResourceReport(int resourceID,
                                               const vector<BorrowRecord>& records) const
{
    Validator::validateID(resourceID);

    ostringstream report;
    report << "===== RESOURCE REPORT =====\n";
    report << "Generated on: " << getCurrentTimestamp() << "\n";
    report << "Resource ID: " << resourceID << "\n";
    report << "=====================================\n\n";

    bool found = false;

    for (const auto& record : records)
    {
        if (record.getResourceID() == resourceID)
        {
            found = true;

            report << "Record ID   : " << record.getRecordID() << "\n";
            report << "User ID     : " << record.getUserID() << "\n";
            report << "Issue Date  : " << record.getIssueDate() << "\n";
            report << "Due Date    : " << record.getDueDate() << "\n";
            report << "Return Date : " << record.getReturnDate() << "\n";
            report << "Fine        : $" << fixed << setprecision(2) << record.getFineAmount() << "\n";
            report << "---------------------------\n";
        }
    }

    if (!found)
        throw EmptyReportException("No borrow records found for this resource");

    return report.str();
}

// -------- OVERALL BORROW REPORT --------

string ReportGenerator::generateOverallBorrowReport(const vector<BorrowRecord>& records) const
{
    ostringstream report;
    report << "===== OVERALL BORROW REPORT =====\n";
    report << "Generated on: " << getCurrentTimestamp() << "\n";
    report << "Total Records: " << records.size() << "\n";
    report << "=====================================\n\n";

    double totalFines = 0;
    int totalReturned = 0;
    int totalPending = 0;

    for (const auto& record : records)
    {
        report << "Record ID   : " << record.getRecordID() << "\n";
        report << "User ID     : " << record.getUserID() << "\n";
        report << "Resource ID : " << record.getResourceID() << "\n";
        report << "Issue Date  : " << record.getIssueDate() << "\n";
        report << "Due Date    : " << record.getDueDate() << "\n";
        report << "Return Date : " << record.getReturnDate() << "\n";
        
        string status = record.getReturnDate().empty() ? "Pending" : "Returned";
        report << "Status      : " << status << "\n";
        report << "Fine        : $" << fixed << setprecision(2) << record.getFineAmount() << "\n";
        report << "---------------------------\n";

        totalFines += record.getFineAmount();
        if (status == "Returned") totalReturned++;
        else totalPending++;
    }

    report << "\n===== SUMMARY =====\n";
    report << "Total Returned: " << totalReturned << "\n";
    report << "Total Pending: " << totalPending << "\n";
    report << "Total Fines: $" << fixed << setprecision(2) << totalFines << "\n";

    return report.str();
}

// -------- OVERDUE REPORT --------

string ReportGenerator::generateOverdueReport(const vector<BorrowRecord>& records) const
{
    ostringstream report;
    report << "===== OVERDUE REPORT =====\n";
    report << "Generated on: " << getCurrentTimestamp() << "\n";
    report << "=====================================\n\n";

    bool found = false;
    double totalOverdueFines = 0;

    for (const auto& record : records)
    {
        if (record.getStatus() == "Overdue")
        {
            found = true;

            report << "Record ID   : " << record.getRecordID() << "\n";
            report << "User ID     : " << record.getUserID() << "\n";
            report << "Resource ID : " << record.getResourceID() << "\n";
            report << "Due Date    : " << record.getDueDate() << "\n";
            report << "Fine        : $" << fixed << setprecision(2) << record.getFineAmount() << "\n";
            report << "---------------------------\n";

            totalOverdueFines += record.getFineAmount();
        }
    }

    report << "\nTotal Overdue Fines: $" << fixed << setprecision(2) << totalOverdueFines << "\n";

    if (!found)
        report << "No overdue records found.\n";

    return report.str();
}
