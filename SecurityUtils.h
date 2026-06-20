#ifndef SECURITYUTILS_H
#define SECURITYUTILS_H

#include <string>
#include <sstream>  // to manipulate strings for hashing and formatting
#include <iomanip>  // for output formatting
#include <cstdint>  // for fixed-width integer types

class SecurityUtils {
public:
    // Generate a random salt for password hashing
    static std::string generateSalt(size_t length = 16);
    
    // Hash password with salt (SHA256 based)
    static std::string hashPassword(const std::string& password, const std::string& salt);
    
    // Verify password against stored hash
    static bool verifyPassword(const std::string& password, const std::string& salt, const std::string& storedHash);
    
    // Create secure storage string (salt:hash format)      
    static std::string createSecurePassword(const std::string& password);
    
    // Extract salt from secure password string
    static std::string extractSalt(const std::string& securePassword);
    
    // Extract hash from secure password string
    static std::string extractHash(const std::string& securePassword);
    
    // Mask sensitive data for display
    static std::string maskSensitiveData(const std::string& data, size_t visibleChars = 4);    
    
    // Generate secure random ID
    static int generateSecureID();     
    
    // Validate admin registration key format
    static bool validateAdminKey(const std::string& key);

    // Verify shared admin access code provided by authorized admin hires
    static bool verifyAdminAccessCode(const std::string& code);
    
    // Get current timestamp for audit logs
    static std::string getCurrentTimestamp();
    
    // Hash string using SHA256
    static std::string sha256(const std::string& input);
    
private:
    static uint64_t hashCombine(uint64_t h1, uint64_t h2);
    static std::string bytesToHex(const uint8_t* bytes, size_t len);
};

#endif
