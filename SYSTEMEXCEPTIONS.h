// custom exceptions for the library management system
#ifndef SYSTEM_EXCEPTIONS_H         // header guard to prevent multiple inclusions
#define SYSTEM_EXCEPTIONS_H

#include <stdexcept>
#include <string>

// all exceptions inherit from AppException, which inherits from std::runtime_error
class AppException : public std::runtime_error {
public:
    explicit AppException(const std::string& msg)       // explicit constructor to prevent implicit conversions
        : std::runtime_error(msg) {}
};

class ValidationException : public AppException {
public:
    explicit ValidationException(const std::string& msg)
        : AppException("Validation Error:\n" + msg) {}       // + concatenation of "Validation Error:\n" with the provided message for clarity
};

class OperationCancelledException : public AppException {
public:
    explicit OperationCancelledException(const std::string& msg = "Operation cancelled by user.")   // default message if no specific message is provided
        : AppException(msg) {}
};

class AuthenticationException : public AppException {
public:
    explicit AuthenticationException(const std::string& msg = "Authentication failed: Invalid credentials.")
        : AppException(msg) {}
};

class NotFoundException : public AppException {
public:
    explicit NotFoundException(const std::string& msg)
        : AppException("Not Found:\n" + msg) {}
};

class AuthorizationException : public AppException {
public:
    explicit AuthorizationException(const std::string& msg)
        : AppException("Authorization Error:\n" + msg) {}
};

class ResourceStateException : public AppException {
public:
    explicit ResourceStateException(const std::string& msg)
        : AppException("Resource Error:\n" + msg) {}
};

class BalanceException : public AppException {
public:
    explicit BalanceException(const std::string& msg)
        : AppException("Balance Error:\n" + msg) {}
};

class InvalidDateException : public AppException {
public:
    explicit InvalidDateException(const std::string& msg)
        : AppException(msg) {}
};

class AlreadyReturnedException : public AppException {
public:
    explicit AlreadyReturnedException(const std::string& msg)
        : AppException(msg) {}
};

class InvalidFineException : public AppException {
public:
    explicit InvalidFineException(const std::string& msg)
        : AppException(msg) {}
};

class EmptyReportException : public AppException {
public:
    explicit EmptyReportException(const std::string& msg)
        : AppException(msg) {}
};

#endif
