#include "Validator.h"
#include "SYSTEMEXCEPTIONS.h"
#include <regex>    // pattern matching for validation
#include <limits>       // for numeric limits in parseInt
#include <algorithm>    // for transform and find

// ---------------- MACRO CATEGORY MAPPING ----------------

/* This function maps detailed categories to broader macro categories for better organization 
and filtering in the application.*/

std::string Validator::getMacroCategory(const std::string& detailedCategory) {
    std::string cat = detailedCategory;
    // Convert to lowercase for comparison
    std::transform(cat.begin(), cat.end(), cat.begin(), ::tolower);
    // two,  cat.begin() -> start and cat.end() -> to store.
    
    // Computer Science & IT categories
    if (cat == "software engineering" || cat == "programming" || cat == "computer science" ||
        cat == "networking" || cat == "operating systems" || cat == "databases" ||
        cat == "computer architecture" || cat == "compilers" || cat == "web development" ||
        cat == "devops" || cat == "cybersecurity" || cat == "computer graphics" ||
        cat == "cloud computing" || cat == "project management" || cat == "interview preparation") {
        return "Computer Science & IT";
    }
    
    // Data Science & Artificial Intelligence
    if (cat == "artificial intelligence" || cat == "data science") {
        return "Data Science & Artificial Intelligence";
    }
    
    // Fiction
    if (cat == "fiction") {
        return "Fiction";
    }
    
    // Non-Fiction
    if (cat == "non-fiction") {
        return "Non-Fiction";
    }
    
    // Science & Technology - Keep strict for actual science books only
    // Exclude computer science technical books that were incorrectly categorized
    if (cat == "science and technology") {
        return "Science & Technology";
    }
    
    // History & Biography
    if (cat == "history and biography") {
        return "History & Biography";
    }
    
    // Self-Help & Personal Development
    if (cat == "self-help") {
        return "Self-Help & Personal Development";
    }
    
    // Philosophy & Religion
    if (cat == "religion and philosophy") {
        return "Philosophy & Religion";
    }
    
    // Default: return as-is if no mapping found
    return detailedCategory;
}

// returns a list of valid macro categories for use in validation and dropdowns.
std::vector<std::string> Validator::getMacroCategoryList() {
    return {
        "Computer Science & IT",
        "Data Science & Artificial Intelligence",
        "Fiction",
        "Non-Fiction",
        "Science & Technology",
        "History & Biography",
        "Self-Help & Personal Development",
        "Philosophy & Religion"
    };
}

// Checks if the provided macro category is valid by comparing it against the list of known macro categories.
bool Validator::isValidMacroCategory(const std::string& macroCat) {
    std::vector<std::string> validCategories = getMacroCategoryList();
    return std::find(validCategories.begin(), validCategories.end(), macroCat) != validCategories.end();
}

using namespace std;

namespace {
string trim(const string& value)    // Helper function to trim leading and trailing whitespace from a string, used in various validation functions to ensure clean input.
{
    const size_t first = value.find_first_not_of(" \t\r\n");
    if (first == string::npos)
        return "";

    const size_t last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

// Helper function to determine if a given year is a leap year, used in date validation to correctly handle February 29th.
bool isLeapYear(int year)
{
    return (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
}
}

// ---------------- INPUT PARSING & BASIC VALIDATION ----------------


/* The following functions perform various input parsing and validation tasks,
throwing ValidationException with clear messages when validation fails. 
This ensures that all user input is checked for correctness and provides feedback
on what needs to be corrected.*/

int Validator::parseInt(const string& input)
{
    const string cleaned = trim(input);
    if (cleaned.empty())
        throw ValidationException("Please enter a number.");

    size_t processed = 0;
    long long value = 0;

    try {
        value = stoll(cleaned, &processed);         // stoll converts string to long long and stores the number of characters processed in 'processed' for validation against extra characters.
    }
    catch (...) {
        throw ValidationException("Please enter a valid whole number (e.g., 123).");
    }

    if (processed != cleaned.size())
        throw ValidationException("Please enter a valid whole number without extra characters.");

    if (value < numeric_limits<int>::min() || value > numeric_limits<int>::max())
        throw ValidationException("The number is too large or too small. Please enter a reasonable value.");

    return static_cast<int>(value);         // Safely cast to int after range check.
}

void Validator::validateMenuChoice(int choice, int minValue, int maxValue)
{
    if (choice < minValue || choice > maxValue)
        throw ValidationException(
            "Please choose a number between " + to_string(minValue) +
            " and " + to_string(maxValue) + ".");
}

void Validator::validateID(int id)
{
    if (id <= 0)
        throw ValidationException("ID must be a positive number.");
}

void Validator::validatePositiveOrZero(int id)
{
    if (id < 0)
        throw ValidationException("Value must be zero or a positive number.");
}

void Validator::validateMenuChoiceWithZero(int id)
{
    if (id < 0)
        throw ValidationException("Please enter a valid non-negative number.");
}

void Validator::validateString(const string& str)
{
    const string cleaned = trim(str);

    if (cleaned.empty())
        throw ValidationException("This field cannot be left blank. Please enter a value.");

    if (cleaned.length() > 100)
        throw ValidationException("Input is too long. Please keep it under 100 characters.");
}

void Validator::validateName(const string& name)
{
    const string cleaned = trim(name);
    const regex pattern(R"(^(?=.{3,50}$)[A-Za-z]+([ .'-][A-Za-z]+)*$)");

    if (!regex_match(cleaned, pattern))
        throw ValidationException("Name must be 3-50 characters and may include spaces, apostrophes, hyphens, or periods.");
}

void Validator::validateUsername(const string& username)
{
    const string usernameRule =
        "Username must be 6-30 characters, start with a letter, and contain only letters, numbers, underscores, or dots.";

    if (username.length() < 6 || username.length() > 30)
        throw ValidationException(usernameRule);

    if (!isalpha(static_cast<unsigned char>(username[0])))
        throw ValidationException(usernameRule);

    regex pattern(R"(^[A-Za-z][A-Za-z0-9._]{5,29}$)");
    if (!regex_match(username, pattern))
        throw ValidationException(usernameRule);
}

void Validator::validateAddress(const string& address)
{
    const string cleaned = trim(address);
    const regex pattern(R"(^[A-Za-z0-9][A-Za-z0-9\s,.#/\-]{4,149}$)");

    if (!regex_match(cleaned, pattern))
        throw ValidationException("Please enter a valid address (5-150 characters) with street name, house number, or location details.");
}

void Validator::validateCategory(const string& category)
{
    const string cleaned = trim(category);
    const regex pattern(R"(^[A-Za-z][A-Za-z0-9&,'()\/ -]{1,49}$)");

    if (!regex_match(cleaned, pattern))
        throw ValidationException("Please enter a valid category name (2-50 characters) like 'Fiction', 'Computer Science', or 'History'.");
}

void Validator::validateDate(const string& date)
{
    regex pattern(R"(^(\d{4})-(\d{2})-(\d{2})$)");
    smatch match;

    if (!regex_match(date, match, pattern))
        throw ValidationException("Please enter date in YYYY-MM-DD format (e.g., 2024-05-15).");

    const int year = stoi(match[1].str());      // stoi converts string to integer, and match[1], match[2], match[3] correspond to the year, month, and day captured by the regex groups.
    const int month = stoi(match[2].str());
    const int day = stoi(match[3].str());

    if (year < 1900 || year > 2100)
        throw ValidationException("Year must be between 1900 and 2100.");

    if (month < 1 || month > 12)
        throw ValidationException("Month must be between 01 and 12.");

    // Check month length so impossible dates such as 2026-02-31 are rejected.
    int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (month == 2 && isLeapYear(year))
        daysInMonth[1] = 29;

    if (day < 1 || day > daysInMonth[month - 1])
        throw ValidationException("Day is not valid for the given month and year.");
}

void Validator::validateISBN(const string& isbn)
{
    const string cleaned = trim(isbn);
    regex pattern(R"(^(?:\d{10}|\d{13})$)");

    if (!regex_match(cleaned, pattern))
        throw ValidationException("ISBN must be exactly 10 or 13 digits (no spaces or hyphens).");
}

void Validator::validateUrl(const string& url)
{
    const string cleaned = trim(url);
    const regex pattern(R"(^(https?:\/\/)[A-Za-z0-9.-]+\.[A-Za-z]{2,}(\/\S*)?$)");

    if (!regex_match(cleaned, pattern))
        throw ValidationException("Please enter a valid download link starting with http:// or https://.");
}

void Validator::validateFileFormat(const string& format)
{
    const string cleaned = trim(format);
    const regex pattern(R"(^[A-Za-z0-9]{2,10}$)");

    if (!regex_match(cleaned, pattern))
        throw ValidationException("File format must be 2-10 characters (e.g., PDF, EPUB, DOCX).");
}

void Validator::validatePages(int pages)
{
    if (pages <= 0)
        throw ValidationException("Number of pages must be greater than 0.");

    if (pages > 10000)
        throw ValidationException("Number of pages seems unrealistically high. Please verify the value.");
}

void Validator::validateFileSize(float size)
{
    if (size <= 0)
        throw ValidationException("File size must be greater than 0 MB.");

    if (size > 102400)
        throw ValidationException("File size is too large. Maximum allowed size is 102,400 MB.");
}

void Validator::validateEmail(const string& email)
{
    const string cleaned = trim(email);
    regex pattern(R"(^[a-z0-9._%+-]+@[a-z0-9.-]+\.[a-z]{2,}$)");

    if (cleaned.length() > 254 || !regex_match(cleaned, pattern))
        throw ValidationException("Please enter a valid email address in lowercase (e.g., name@example.com).");
}

void Validator::validateCNIC(const string& cnic)
{
    regex pattern(R"(^\d{5}-\d{7}-\d{1}$)");

    if (!regex_match(cnic, pattern))
        throw ValidationException("CNIC must be in the format XXXXX-XXXXXXX-X (e.g., 12345-1234567-1).");
}

void Validator::validatePhone(const string& phone)
{
    regex pattern(R"(^03\d{9}$)");

    if (!regex_match(phone, pattern))
        throw ValidationException("Phone number must be 11 digits starting with 03 (e.g., 03001234567).");
}

void Validator::validatePassword(const string& password)
{
    const string rule =
        "Password must be 8-64 characters with at least one uppercase letter, one lowercase letter, one number, and one special character. Spaces are not allowed.";

    if (password.length() < 8 || password.length() > 64)
        throw ValidationException(rule);

    bool hasUpper = false;
    bool hasLower = false;
    bool hasDigit = false;
    bool hasSymbol = false;

    for (unsigned char c : password)
    {
        if (isspace(c))
            throw ValidationException(rule);

        if (isupper(c)) hasUpper = true;
        else if (islower(c)) hasLower = true;
        else if (isdigit(c)) hasDigit = true;
        else hasSymbol = true;
    }

    if (!hasUpper || !hasLower || !hasDigit || !hasSymbol)
        throw ValidationException(rule);
}
