#include "Admin.h"
#include "SecurityUtils.h"
#include <iostream>
#include <sstream>

using namespace std;

Admin::Admin(int id, string username, string password,
             string first, string last, string email,
             string cnic, string phone, string address,
             double balance,
             AdminRole role)
    : User(id, username, password, first, last, email, cnic, phone, address, balance)
{
    roleType_ = role;
    isActive_ = true;
    role_ = "Admin";
    createdAt_ = SecurityUtils::getCurrentTimestamp();
    lastLogin_ = "";
}

void Admin::displayProfile() const {
    cout << "\n==========================================\n";
    cout << "    ADMIN PROFILE\n";
    cout << "==========================================\n";
    cout << "User ID          : " << userID_ << '\n';
    cout << "Role             : " << role_ << '\n';
    cout << "Username         : " << username_ << '\n';
    cout << "Full Name        : " << firstName_ << ' ' << lastName_ << '\n';
    cout << "Email            : " << email_ << '\n';
    cout << "CNIC             : " << cnic_ << '\n';
    cout << "Phone            : " << phoneNumber_ << '\n';
    cout << "Address          : " << address_ << '\n';
    cout << "Admin Role       : " << getAdminRoleName() << '\n';
    cout << "Account Status   : " << (isActive_ ? "Active" : "Suspended") << '\n';
    cout << "Outstanding Fine : " << balance_ << '\n';
}

void Admin::displayFullProfile() const {
    cout << "\n==========================================\n";
    cout << "    FULL ADMIN PROFILE\n";
    cout << "==========================================\n";
    cout << "User ID          : " << userID_ << '\n';
    cout << "Role             : " << role_ << '\n';
    cout << "Username         : " << username_ << '\n';
    cout << "Full Name        : " << firstName_ << ' ' << lastName_ << '\n';
    cout << "Email            : " << email_ << '\n';
    cout << "CNIC             : " << cnic_ << '\n';
    cout << "Phone            : " << phoneNumber_ << '\n';
    cout << "Address          : " << address_ << '\n';
    cout << "Admin Role       : " << getAdminRoleName() << '\n';
    cout << "Account Status   : " << (isActive_ ? "Active" : "Suspended") << '\n';
    cout << "Outstanding Fine : " << balance_ << '\n';
    cout << "Created At       : " << createdAt_ << '\n';
    cout << "Last Login       : " << (lastLogin_.empty() ? "Never" : lastLogin_) << '\n';
    cout << "Password Status  : " << (isPasswordSecure() ? "Secure" : "Needs Update") << '\n';
}

string Admin::serialize() const {   // Convert admin object to a string for file storage, using '|' as a delimiter
    string roleStr;
    switch (roleType_) {
        case AdminRole::SUPER_ADMIN: roleStr = "ROLE_SUPER_ADMIN"; break;
        case AdminRole::LIBRARIAN: roleStr = "ROLE_LIBRARIAN"; break;
        case AdminRole::NOTIFICATION_ADMIN: roleStr = "ROLE_NOTIFICATION_ADMIN"; break;
        case AdminRole::FINE_MANAGER: roleStr = "ROLE_FINE_MANAGER"; break;
        default: roleStr = "ROLE_ADMIN"; break;
    }

    string activeStr = isActive_ ? "ACTIVE" : "SUSPENDED";

    // Format: Admin|userID|username|password|firstName|lastName|email|cnic|phone|address|balance|role|activeStatus|createdAt|lastLogin
    return "Admin|" + to_string(userID_) + "|" + username_ + "|" +
           password_ + "|" + firstName_ + "|" + lastName_ + "|" +
           email_ + "|" + cnic_ + "|" + phoneNumber_ + "|" +
           address_ + "|" + to_string(balance_) +
           "|" + roleStr + "|" + activeStr + "|" + createdAt_ + "|" + lastLogin_;
}

Admin* Admin::deserialize(const string& line) { // Convert a string from file storage back into an Admin object
    stringstream ss(line);   // Use stringstream to parse the line with '|' as a delimiter
    string token;   // token variable for parsing

    getline(ss, token, '|'); // Admin

    int id;
    string username, password, first, last, email, cnic, phone, address;
    string roleStr, activeStr;
    double balance;

    getline(ss, token, '|'); id = stoi(token); // stoi converts string to int
    getline(ss, username, '|');
    getline(ss, password, '|');
    getline(ss, first, '|');
    getline(ss, last, '|');
    getline(ss, email, '|');
    getline(ss, cnic, '|');
    getline(ss, phone, '|');
    getline(ss, address, '|');
    getline(ss, token, '|'); balance = stod(token);     // stod converts string to double
    
    // Handle backward compatibility for old format with department
    string nextToken;
    getline(ss, nextToken, '|');
    
    // Check if this is the old format with department
    if (!ss.eof()) {
        string tempCheck;
        size_t checkPos = ss.tellg();
        if (getline(ss, tempCheck, '|')) {
            // Check if tempCheck looks like a role string
            if (tempCheck.find("ROLE_") != string::npos || tempCheck == "SUPER_ADMIN" || tempCheck.empty()) {
                // Old format with department - nextToken is department, tempCheck is role
                roleStr = tempCheck;
            } else {
                // New format - nextToken is role
                roleStr = nextToken;
                ss.seekg(checkPos);
            }
        }
    } else {
        roleStr = nextToken;
    }
    
    getline(ss, activeStr, '|');
    string createdAt;
    string lastLogin;
    getline(ss, createdAt, '|');
    getline(ss, lastLogin, '|');

    AdminRole roleType;
    if (roleStr == "ROLE_SUPER_ADMIN")
        roleType = AdminRole::SUPER_ADMIN;
    else if (roleStr == "ROLE_LIBRARIAN")
        roleType = AdminRole::LIBRARIAN;
    else if (roleStr == "ROLE_NOTIFICATION_ADMIN")
        roleType = AdminRole::NOTIFICATION_ADMIN;
    else if (roleStr == "ROLE_FINE_MANAGER")
        roleType = AdminRole::FINE_MANAGER;
    else if (roleStr == "SUPER_ADMIN")
        roleType = AdminRole::SUPER_ADMIN; // backward compatibility
    else
        roleType = AdminRole::ADMIN; // backward compatibility and default

    Admin* admin = new Admin(id, username, password, first, last,
                          email, cnic, phone, address, balance, roleType);
    admin->createdAt_ = createdAt;
    admin->lastLogin_ = lastLogin;
    if (activeStr == "SUSPENDED") {
        admin->suspend();
    }

    return admin;
}

void Admin::setPrivilegeLevel(AdminRole role) {
    roleType_ = role;
}

AdminRole Admin::getPrivilegeLevel() const {
    return roleType_;
}

std::string Admin::getAdminRoleName() const {
    switch (roleType_) {
        case AdminRole::SUPER_ADMIN: return "Super Admin";
        case AdminRole::LIBRARIAN: return "Librarian";
        case AdminRole::NOTIFICATION_ADMIN: return "Notification Admin";
        case AdminRole::FINE_MANAGER: return "Fine Manager";
        default: return "Admin";
    }
}

bool Admin::hasSuperAdminPrivilege() const {
    return roleType_ == AdminRole::SUPER_ADMIN && isActive_;
}

bool Admin::canManageMembers() const {
    return isActive_ && (roleType_ == AdminRole::SUPER_ADMIN || roleType_ == AdminRole::ADMIN);
}

bool Admin::canManageAdmins() const {
    return roleType_ == AdminRole::SUPER_ADMIN && isActive_;
}

bool Admin::canManageResources() const {
    return isActive_ && (roleType_ == AdminRole::SUPER_ADMIN || roleType_ == AdminRole::LIBRARIAN);
}

bool Admin::canManageNotifications() const {
    return isActive_ && (roleType_ == AdminRole::SUPER_ADMIN || roleType_ == AdminRole::NOTIFICATION_ADMIN);
}

bool Admin::canManageFines() const {
    return isActive_ && (roleType_ == AdminRole::SUPER_ADMIN || roleType_ == AdminRole::FINE_MANAGER);
}

void Admin::suspend() {
    isActive_ = false;
}

void Admin::reinstate() {
    isActive_ = true;
}

bool Admin::isActive() const {
    return isActive_;
}

void Admin::updateLastLogin() {
    lastLogin_ = SecurityUtils::getCurrentTimestamp();
}

bool Admin::isPasswordSecure() const {
    // Check if password is hashed (contains ':')
    return password_.find(':') != string::npos;
}
