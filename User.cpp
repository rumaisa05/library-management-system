#include "User.h"
#include "SYSTEMEXCEPTIONS.h"
#include "Validator.h"      // For input validation
#include "SecurityUtils.h"    // For password hashing and verification
#include <algorithm>    // all_of for validating hex digits in password hash.
#include <cctype>       // isxdigit for validating hex digits in password hash.
#include <iostream>
#include <stdexcept>

using namespace std;

namespace {         // anonymous namespace for internal helper functions , not exposed outside this file (.cpp )
bool isStoredPasswordHash(const string& password)   // Checks if the provided password string is in the expected stored hash format (salt:hash) and validates its structure
{
    const size_t separator = password.find(':');    // Expected format: salt:hash
    if (separator == string::npos)      // No separator found, not a valid stored password hash
        return false;

    const string salt = SecurityUtils::extractSalt(password);
    const string hash = SecurityUtils::extractHash(password);

    if (salt.empty() || hash.length() != 64)
        return false;

    return all_of(hash.begin(), hash.end(), [](unsigned char c) {
        return isxdigit(c) != 0;
    });
}

void validateStoredPassword(const string& password)     // Validates that the provided password is either a valid stored hash or a new plaintext password that meets complexity requirements
{
    if (isStoredPasswordHash(password))     // If it's already a valid stored hash, we consider it valid for loading existing users
        return;

    Validator::validatePassword(password);  // If it's not a stored hash, validate it as a new plaintext password 
}
}

User::User(int id, string username, string password,
           string first, string last, string email,
           string cnic, string phone, string address, double balance)
{
    Validator::validateID(id);
    Validator::validateUsername(username);
    validateStoredPassword(password);
    Validator::validateName(first);
    Validator::validateName(last);
    Validator::validateEmail(email);
    Validator::validateCNIC(cnic);
    Validator::validatePhone(phone);
    Validator::validateAddress(address);

    userID_ = id;
    username_ = username;
    password_ = password;
    firstName_ = first;
    lastName_ = last;
    email_ = email;
    cnic_ = cnic;
    phoneNumber_ = phone;
    address_ = address;
    balance_ = balance;
    role_ = "User";     // Base role, will be overridden in derived classes , by default we set it to "User" but Admin and Member classes will set it to "Admin" and "Member" respectively in their constructors
    isActive_ = true;       // New users are active by default
    createdAt_ = SecurityUtils::getCurrentTimestamp();      // Set creation timestamp when user is created
}

bool User::login(const string& username, const string& password) const {
    // Check if account is active
    if (!isActive_) {
        return false;
    }
    
    // Check if password is hashed
    if (password_.find(':') != string::npos) {
        // Hashed password format
        string salt = SecurityUtils::extractSalt(password_);
        string storedHash = SecurityUtils::extractHash(password_);
        
        if (username_ == username && 
            SecurityUtils::verifyPassword(password, salt, storedHash)) {
            return true;
        }
    } else {      // for user accounts created before implementing password hashing
        // Legacy plaintext password
        if (username_ == username && password_ == password) {
            return true;
        }
    }
    
    return false;
}

bool User::loginWithHash(const string& password) const {    // Login method ,focuses on verifying the password hash and account status
    if (!isActive_) {
        return false;
    }
    
    // For hashed passwords
    if (password_.find(':') != string::npos) {
        string salt = SecurityUtils::extractSalt(password_);
        string storedHash = SecurityUtils::extractHash(password_);
        return SecurityUtils::verifyPassword(password, salt, storedHash);
    }
    
    return false;
}

void User::updateBalance(double amount) {
    // Allow balance to go negative to represent outstanding fines
    // Positive balance = Account credit, Negative balance = Outstanding fines
    balance_ += amount;
}

void User::suspend() {      
    isActive_ = false;
}

void User::reinstate() {    // Reactivate a suspended account
    isActive_ = true;
}

bool User::isActive() const {
    return isActive_;
}
 
int User::getUserID() const { return userID_; }
string User::getUsername() const { return username_; }
string User::getRole() const { return role_; }
string User::getFirstName() const { return firstName_; }
string User::getLastName() const { return lastName_; }
string User::getEmail() const { return email_; }
string User::getCNIC() const { return cnic_; }
string User::getPhone() const { return phoneNumber_; }
string User::getAddress() const { return address_; }
double User::getBalance() const { return balance_; }

void User::setEmail(const string& email) { 
    Validator::validateEmail(email);
    email_ = email; 
}

void User::setPhone(const string& phone) { 
    Validator::validatePhone(phone);
    phoneNumber_ = phone; 
}

void User::setAddress(const string& address) { 
    Validator::validateAddress(address);
    address_ = address; 
}

void User::setFirstName(const string& first) { 
    Validator::validateName(first);
    firstName_ = first; 
}

void User::setLastName(const string& last) { 
    Validator::validateName(last);
    lastName_ = last; 
}

bool User::operator==(const User& other) const {
    return userID_ == other.userID_;
}
