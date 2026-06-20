#include "SYSTEMEXCEPTIONS.h"
#include "Usermanager.h"
#include "InputHelper.h"
#include "Validator.h"
#include "SecurityUtils.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace std;

namespace {
int generateNextUserID(const vector<User*>& users) {
    int maxID = 0;

    for (const auto* user : users) {
        if (user && user->getUserID() > maxID)
            maxID = user->getUserID();
    }

    return maxID + 1;
}
}

static double readValidatedBalance(const std::string& prompt) {
    while (true) {
        try {
            const std::string value = InputHelper::readLine(prompt);
            
            // Check if empty, set to 0
            if (value.empty()) {
                return 0.0;
            }
            
            size_t processed = 0;
            const double amount = stod(value, &processed);
            
            // Check if the entire string was processed (numeric check)
            if (processed != value.size()) {
                throw ValidationException("Balance must be a valid number.");
            }
            
            // Check if >= 0
            if (amount < 0.0) {
                throw ValidationException("Balance cannot be negative.");
            }
            
            // Check if <= 5000
            if (amount > 5000.0) {
                throw ValidationException("Balance cannot exceed Rs. 5000.");
            }
            
            // Check decimal places <= 2
            size_t decimalPos = value.find('.');
            if (decimalPos != std::string::npos) {
                size_t decimalPlaces = value.size() - decimalPos - 1;
                if (decimalPlaces > 2) {
                    throw ValidationException("Balance can have at most 2 decimal places.");
                }
            }
            
            return amount;
        }
        catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
            if (!InputHelper::askRetryOrExit())
                throw OperationCancelledException();
        }
    }
}

UserManager::UserManager(const string& file)
    : filename_(file) {
    loadFromFile();
}

UserManager::~UserManager() {
    try {
        saveToFile();
    }
    catch (...) {
    }

    for (auto* user : users_)
        delete user;
}

void UserManager::loadFromFile() {
    ifstream file(filename_);
    if (!file.is_open())
        return;

    for (auto* user : users_)
        delete user;
    users_.clear();

    string line;
    while (getline(file, line)) {
        if (line.empty())
            continue;

        try {
            if (line.find("Member|") == 0)
                users_.push_back(Member::deserialize(line));
            else if (line.find("Admin|") == 0)
                users_.push_back(Admin::deserialize(line));
        }
        catch (const exception&) {
            // Ignore malformed rows
        }
    }
}

void UserManager::saveToFile() {
    ofstream file(filename_);
    if (!file.is_open())
        throw runtime_error("Unable to open user file for saving");

    for (const auto* user : users_)
        file << user->serialize() << '\n';
}

void UserManager::registerMember() {
    const int id = generateNextUserID(users_);
    string username;
    while (true) {
        username = InputHelper::readValidatedLine("Username: ", Validator::validateUsername);

        if (!userExists(username))
            break;

        throw ValidationException("This username is already taken. Please choose a different username.");
    }

    const string password = InputHelper::readValidatedLine("Password: ", Validator::validatePassword);
    const string firstName = InputHelper::readValidatedLine("First Name: ", Validator::validateName);
    const string lastName = InputHelper::readValidatedLine("Last Name: ", Validator::validateName);
    const string email = InputHelper::readValidatedLine("Email: ", Validator::validateEmail);
    const string cnic = InputHelper::readValidatedLine("CNIC (XXXXX-XXXXXXX-X): ", Validator::validateCNIC);
    const string phone = InputHelper::readValidatedLine("Phone (03XXXXXXXXX): ", Validator::validatePhone);
    const string address = InputHelper::readValidatedLine("Address: ", Validator::validateAddress);
    const double startingBalance = readValidatedBalance("Initial account balance for future use (Rs.) [Press Enter for 0]: ");

    // Hash the password for security
    string hashedPassword = SecurityUtils::createSecurePassword(password);

    users_.push_back(new Member(
        id, username, hashedPassword, firstName, lastName, email,
        cnic, phone, address, startingBalance
    ));

    saveToFile();
    cout << "\n" << string(50, '=') << "\n";
    cout << "MEMBER REGISTRATION SUCCESSFUL\n";
    cout << string(50, '=') << "\n";
    cout << "User ID: " << id << "\n";
    cout << "Username: " << username << "\n";
    cout << "Name: " << firstName << " " << lastName << "\n";
    cout << "\nYour account has been created successfully.\n";
    cout << "You can now log in with your credentials.\n\n";
}

void UserManager::registerAdmin(bool forceSuperAdmin) {
    const int id = generateNextUserID(users_);
    string username;
    while (true) {
        username = InputHelper::readValidatedLine("Username: ", Validator::validateUsername);

        if (!userExists(username))
            break;

        throw ValidationException("This username is already taken. Please choose a different username.");
    }

    const string password = InputHelper::readValidatedLine("Password: ", Validator::validatePassword);
    const string firstName = InputHelper::readValidatedLine("First Name: ", Validator::validateName);
    const string lastName = InputHelper::readValidatedLine("Last Name: ", Validator::validateName);
    const string email = InputHelper::readValidatedLine("Email: ", Validator::validateEmail);
    const string cnic = InputHelper::readValidatedLine("CNIC (XXXXX-XXXXXXX-X): ", Validator::validateCNIC);
    const string phone = InputHelper::readValidatedLine("Phone (03XXXXXXXXX): ", Validator::validatePhone);
    const string address = InputHelper::readValidatedLine("Address: ", Validator::validateAddress);
    
    cout << "\nTo register as an admin, you must enter the shared company admin access code.\n";
    cout << "This code is issued by the library system owner and is required for all admin registrations.\n";
    cout << "You have 3 attempts to enter the correct code.\n";
    string accessCode;
    int attempts = 0;
    while (attempts < 3) {
        accessCode = InputHelper::readValidatedLine("Admin access code: ", Validator::validateString);
        if (SecurityUtils::verifyAdminAccessCode(accessCode))
            break;
        attempts++;
        if (attempts >= 3) {
            cout << "Invalid admin access code. Maximum attempts reached. Registration cancelled.\n";
            throw OperationCancelledException();
        }
        cout << "Invalid admin access code. " << (3 - attempts) << " attempt(s) remaining. Please contact the library system owner for the correct code.\n";
    }

    AdminRole roleType;
    
    // If forced to be super admin (system startup)
    if (forceSuperAdmin) {
        roleType = AdminRole::SUPER_ADMIN;
        cout << "\n*** SYSTEM INITIALIZATION ***\n";
        cout << "As the first administrator, you are being registered as a SUPER ADMIN.\n";
        cout << "After this registration, only super admins can register other admins.\n";
    }
    else {
        cout << "\nSelect admin role:\n";
        cout << "1. Standard Admin\n";
        cout << "2. Librarian\n";
        cout << "3. Notification Admin\n";
        cout << "4. Fine Manager\n";
        cout << "5. Super Admin\n";
        int roleChoice = InputHelper::readMenuChoice("Choice: ", 1, 5);

        switch (roleChoice) {
            case 2: roleType = AdminRole::LIBRARIAN; break;
            case 3: roleType = AdminRole::NOTIFICATION_ADMIN; break;
            case 4: roleType = AdminRole::FINE_MANAGER; break;
            case 5:
                if (superAdminExists())
                    throw ValidationException("A super admin already exists. Only one super admin is allowed.");
                roleType = AdminRole::SUPER_ADMIN;
                break;
            default:
                roleType = AdminRole::ADMIN;
                break;
        }
    }

    // Hash the password for security
    string hashedPassword = SecurityUtils::createSecurePassword(password);

    users_.push_back(new Admin(
        id, username, hashedPassword, firstName, lastName, email,
        cnic, phone, address, 0.0, roleType
    ));

    saveToFile();
    cout << "\n" << string(50, '=') << "\n";
    cout << "ADMIN REGISTRATION SUCCESSFUL\n";
    cout << string(50, '=') << "\n";
    cout << "User ID: " << id << "\n";
    cout << "Username: " << username << "\n";
    cout << "Name: " << firstName << " " << lastName << "\n";
    cout << "Role: " << (roleType == AdminRole::SUPER_ADMIN ? "Super Admin" :
                         roleType == AdminRole::LIBRARIAN ? "Librarian" :
                         roleType == AdminRole::NOTIFICATION_ADMIN ? "Notification Admin" :
                         roleType == AdminRole::FINE_MANAGER ? "Fine Manager" : "Admin") << "\n";
    cout << "\nYour admin account has been created successfully.\n\n";
}

User* UserManager::loginUser(const string& username, const string& password) {
    Validator::validateUsername(username);

    for (auto* user : users_) {
        if (user->login(username, password)) {
            // Update last login for admin
            Admin* admin = dynamic_cast<Admin*>(user);
            if (admin) {
                admin->updateLastLogin();
                saveToFile();
            }
            return user;
        }
    }

    throw AuthenticationException();
}

User* UserManager::findUserByID(int id) {
    for (auto* user : users_) {
        if (user->getUserID() == id)
            return user;
    }

    return nullptr;
}

const User* UserManager::findUserByID(int id) const {
    for (auto* user : users_) {
        if (user->getUserID() == id)
            return user;
    }

    return nullptr;
}

User* UserManager::findUserByUsername(const string& username) {
    for (auto* user : users_) {
        if (user->getUsername() == username)
            return user;
    }

    return nullptr;
}

Member* UserManager::findMemberByID(int id) {
    for (auto* user : users_) {
        Member* member = dynamic_cast<Member*>(user);
        if (member && member->getUserID() == id)
            return member;
    }
    return nullptr;
}

Admin* UserManager::findAdminByID(int id) {
    for (auto* user : users_) {
        Admin* admin = dynamic_cast<Admin*>(user);
        if (admin && admin->getUserID() == id)
            return admin;
    }
    return nullptr;
}

bool UserManager::userExists(const string& username) {
    return findUserByUsername(username) != nullptr;
}

const vector<User*>& UserManager::getUsers() const {
    return users_;
}

void UserManager::displayAllMembers() const {
    bool found = false;

    for (const auto* user : users_) {
        if (user && user->getRole() == "Member") {
            // Check if admin viewing - show more details
            const Member* member = dynamic_cast<const Member*>(user);
            if (member) {
                member->displayProfile();
            } else {
                user->displayProfile();
            }
            cout << "----------------------\n";
            found = true;
        }
    }

    if (!found)
        cout << "No members found.\n";
}

void UserManager::displayAllAdmins() const {
    bool found = false;

    for (const auto* user : users_) {
        if (user && user->getRole() == "Admin") {
            const Admin* admin = dynamic_cast<const Admin*>(user);
            if (admin) {
                admin->displayProfile();
            }
            cout << "----------------------\n";
            found = true;
        }
    }

    if (!found)
        cout << "No admins found.\n";
}

void UserManager::displayUserDetails(int userID) const {
    const User* user = findUserByID(userID);
    if (!user) {
        cout << "User not found.\n";
        return;
    }

    const Member* member = dynamic_cast<const Member*>(user);
    const Admin* admin = dynamic_cast<const Admin*>(user);

    if (member) {
        member->displayFullProfile();
    } else if (admin) {
        admin->displayFullProfile();
    } else {
        user->displayProfile();
    }
}

bool UserManager::suspendMember(int userID) {
    Member* member = findMemberByID(userID);
    if (!member) {
        cout << "\nMember not found. Please verify the User ID.\n";
        return false;
    }

    member->suspend();
    saveToFile();
    cout << "\nMember suspended successfully.\n";
    return true;
}

bool UserManager::reinstateMember(int userID) {
    Member* member = findMemberByID(userID);
    if (!member) {
        cout << "\nMember not found.\n";
        return false;
    }

    member->reinstate();
    saveToFile();
    cout << "\nMember reinstated successfully.\n";
    return true;
}

bool UserManager::deleteMember(int userID) {
    for (auto it = users_.begin(); it != users_.end(); ++it) {
        Member* member = dynamic_cast<Member*>(*it);
        if (member && member->getUserID() == userID) {
            delete *it;
            users_.erase(it);
            saveToFile();
            cout << "\nMember deleted successfully.\n";
            return true;
        }
    }
    cout << "\nMember not found.\n";;
    return false;
}

bool UserManager::adjustMemberBalance(int userID, double amount) {
    Member* member = findMemberByID(userID);
    if (!member) {
        cout << "Member not found.\n";
        return false;
    }

    member->updateBalance(amount);
    saveToFile();
    cout << "\nMember balance adjusted successfully.\n";
    return true;
}

bool UserManager::editMember(int userID, const string& field, const string& newValue) {
    Member* member = findMemberByID(userID);
    if (!member) {
        throw NotFoundException("Member with ID " + to_string(userID) + " not found.");
    }

    try {
        if (field == "email") {
            member->setEmail(newValue);
        } else if (field == "phone") {
            member->setPhone(newValue);
        } else if (field == "address") {
            member->setAddress(newValue);
        } else if (field == "firstname") {
            member->setFirstName(newValue);
        } else if (field == "lastname") {
            member->setLastName(newValue);
        } else {
            throw ValidationException("Invalid field '" + field + "'. Valid fields are: email, phone, address, firstname, lastname.");
        }
        saveToFile();
        cout << "\nMember updated successfully.\n";
        return true;
    } catch (const exception& e) {
        cout << "Error: " << e.what() << "\n";
        return false;
    }
}

bool UserManager::suspendAdmin(int userID) {
    Admin* admin = findAdminByID(userID);
    if (!admin) {
        cout << "Admin not found.\n";
        return false;
    }

    admin->suspend();
    saveToFile();
    cout << "\nAdmin suspended successfully.\n";
    return true;
}

bool UserManager::reinstateAdmin(int userID) {
    Admin* admin = findAdminByID(userID);
    if (!admin) {
        cout << "Admin not found.\n";
        return false;
    }

    admin->reinstate();
    saveToFile();
    cout << "\nAdmin reinstated successfully.\n";
    return true;
}

bool UserManager::deleteAdmin(int userID) {
    for (auto it = users_.begin(); it != users_.end(); ++it) {
        Admin* admin = dynamic_cast<Admin*>(*it);
        if (admin && admin->getUserID() == userID) {
            delete *it;
            users_.erase(it);
            saveToFile();
            cout << "Admin deleted successfully.\n";
            return true;
        }
    }
    cout << "\nAdmin not found.\n";
    return false;
}

int UserManager::getMemberCount() const {
    int count = 0;
    for (const auto* user : users_) {
        if (user && user->getRole() == "Member")
            count++;
    }
    return count;
}

int UserManager::getAdminCount() const {
    int count = 0;
    for (const auto* user : users_) {
        if (user && user->getRole() == "Admin")
            count++;
    }
    return count;
}

bool UserManager::superAdminExists() const {
    for (const auto* user : users_) {
        const Admin* admin = dynamic_cast<const Admin*>(user);
        if (admin && admin->getPrivilegeLevel() == AdminRole::SUPER_ADMIN)
            return true;
    }
    return false;
}

bool UserManager::adminExists() const {
    for (const auto* user : users_) {
        const Admin* admin = dynamic_cast<const Admin*>(user);
        if (admin)
            return true;
    }
    return false;
}
