#ifndef REPORTGENERATOR_H
#define REPORTGENERATOR_H

#include <string>
#include <vector>
#include "BorrowRecord.h"

class ReportGenerator {
public:
    ReportGenerator();

    // Text Report Generation
    std::string generateUserReport(int userID,
                                   const std::vector<BorrowRecord>& records) const;

    std::string generateResourceReport(int resourceID,
                                       const std::vector<BorrowRecord>& records) const;

    std::string generateOverallBorrowReport(const std::vector<BorrowRecord>& records) const;

    std::string generateOverdueReport(const std::vector<BorrowRecord>& records) const;

};

#endif