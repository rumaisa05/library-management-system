#include "FineManager.h"
#include "SYSTEMEXCEPTIONS.h"
#include "Validator.h"
#include <iostream>

using namespace std;

// ---------------- CONSTRUCTOR ----------------

FineManager::FineManager(double rate) {
    if (rate < 0)
        throw InvalidFineException("Fine rate cannot be negative");

    finePerDay_ = rate;
}

// ---------------- CALCULATE FINE ----------------

double FineManager::calculateFine(int daysLate) const {
    if (daysLate < 0)
        throw InvalidFineException("Days late cannot be negative");

    return daysLate * finePerDay_;
}

// ---------------- DEDUCT FINE ----------------

void FineManager::deductFine(int userID, double amount) {
    Validator::validateID(userID);

    if (amount < 0)
        throw InvalidFineException("Fine amount cannot be negative");

    // NOTE: In real system this should call UserManager or User class
    cout << "Fine Rs. " << amount
         << " applied to User ID: " << userID << endl;
}
