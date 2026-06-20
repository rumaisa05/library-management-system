#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <vector>
#include <string>
#include "User.h"
#include "Member.h"
#include "Admin.h"

class UserManager {
private:
    std::vector<User*> users_;
    std::string filename_;

public:
    UserManager(const std::string& file);
    ~UserManager();

    void loadFromFile();
    void saveToFile();

    // User registration
    void registerMember();
    void registerAdmin(bool forceSuperAdmin = false);

    // Login with enhanced security
    User* loginUser(const std::string& username, const std::string& password);

// User lookup
    User* findUserByID(int id);
    const User* findUserByID(int id) const;
    User* findUserByUsername(const std::string& username);
    Member* findMemberByID(int id);
    Admin* findAdminByID(int id);

    bool userExists(const std::string& username);
    const std::vector<User*>& getUsers() const;

    // Display methods
    void displayAllMembers() const;
    void displayAllAdmins() const;
    void displayUserDetails(int userID) const;

    // Member management (Admin functions)
    bool suspendMember(int userID);
    bool reinstateMember(int userID);
    bool deleteMember(int userID);
    bool editMember(int userID, const std::string& field, const std::string& newValue);
    
    // Admin management
    bool suspendAdmin(int userID);
    bool reinstateAdmin(int userID);
    bool deleteAdmin(int userID);
    bool adjustMemberBalance(int userID, double amount);
    bool superAdminExists() const;
    int getAdminCount() const;
    bool adminExists() const;  // Check if ANY admin exists
    
    // Get counts
    int getMemberCount() const;
};

#endif
