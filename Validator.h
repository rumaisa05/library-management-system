#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <stdexcept>
#include <string>
#include <cctype>
#include <vector>   // dynamic arrays of multiple data types ( int , string ) , template class

// static functions because we don't need to create an instance of Validator to use them, we can call them directly on the class itself eg Validator::validateEmail(email);
class Validator {
public:
    // ---------------- MACRO CATEGORIES ----------------
    static std::string getMacroCategory(const std::string& detailedCategory);
    static std::vector<std::string> getMacroCategoryList();
    static bool isValidMacroCategory(const std::string& macroCat);

    // ---------------- INPUT PARSING ----------------
    static int parseInt(const std::string& input);          // Parses a string into an integer with validation and error handling
    static void validateMenuChoice(int choice, int minValue, int maxValue);

// ---------------- BASIC VALIDATION ----------------
    static void validateID(int id);
    static void validatePositiveOrZero(int id);         // Validates that a number is zero or positive, used for fields like balance or fines
    static void validateMenuChoiceWithZero(int id);     // Validates that a menu choice is zero or a positive number, allowing for "0" as a valid option for exit or back
    static void validateString(const std::string& str);
    static void validateName(const std::string& name);
    static void validateUsername(const std::string& username);
    static void validateAddress(const std::string& address);
    static void validateDate(const std::string& date);
    static void validateCategory(const std::string& category);      // Validates that a category name is well-formed (e.g., "Fiction", "Computer Science") and does not contain invalid characters
    static void validateUrl(const std::string& url);
    static void validateFileFormat(const std::string& format);

    // ---------------- RESOURCE VALIDATION ----------------
    static void validateISBN(const std::string& isbn);
    static void validatePages(int pages);
    static void validateFileSize(float size);

    // ---------------- USER VALIDATION ----------------
    static void validateEmail(const std::string& email);
    static void validateCNIC(const std::string& cnic);
    static void validatePhone(const std::string& phone);
    static void validatePassword(const std::string& password);
};

#endif
