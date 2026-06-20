#ifndef USER_H
#define USER_H

#include <string>

class User {        // abstract base class for Admin and Member
protected:
    int userID_;
    std::string username_;
    std::string password_;
    std::string firstName_;
    std::string lastName_;
    std::string email_;
    std::string cnic_;
    std::string phoneNumber_;
    std::string address_;
    double balance_;
    std::string role_;      // "Admin" or "Member"
    bool isActive_;  // For member suspension    
    std::string createdAt_;

public:
    User(int id, std::string username, std::string password,
         std::string first, std::string last, std::string email,
         std::string cnic, std::string phone, std::string address,
         double balance);

    virtual ~User() = default ;     // Virtual destructor for proper cleanup in derived classes

    // Core
    virtual void displayProfile() const = 0;
    virtual std::string serialize() const = 0;      // For file storage

    // Login with secure password handling
    virtual bool login(const std::string& username, const std::string& password) const;
    bool loginWithHash(const std::string& password) const;

    // Balance
    void updateBalance(double amount);

    // Account status management
    void suspend();
    void reinstate();
    bool isActive() const;
    
    // Getters
    int getUserID() const;
    std::string getUsername() const;
    std::string getRole() const;
    std::string getFirstName() const;
    std::string getLastName() const;
    std::string getEmail() const;
    std::string getCNIC() const;
    std::string getPhone() const;
    std::string getAddress() const;
    double getBalance() const;
    
    // Setters (for editing)
    void setEmail(const std::string& email);
    void setPhone(const std::string& phone);
    void setAddress(const std::string& address);
    void setFirstName(const std::string& first);
    void setLastName(const std::string& last);

    bool operator==(const User& other) const;   // For comparing two users using userID_
};

#endif
