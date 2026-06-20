#include "Notification.h"
#include <sstream>
#include <iostream>
#include <ctime>

namespace {
    std::string getCurrentDate() {
        time_t now = time(nullptr);
        char buf[11];
        strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&now));
        return std::string(buf);
    }

}

Notification::Notification()
    : notificationID_(0), userID_(0), resourceID_(0),
      message_(""), type_(NotificationType::BOOK_AVAILABLE),
      isRead_(false), dateCreated_(getCurrentDate()) {
}

Notification::Notification(int id, int userID, int resourceID,
                             const std::string& message, NotificationType type,
                             const std::string& dateCreated)
    : notificationID_(id), userID_(userID), resourceID_(resourceID),
      message_(message), type_(type), isRead_(false), dateCreated_(dateCreated) {
}

void Notification::setRead(bool read) {
    isRead_ = read;
}

int Notification::getNotificationID() const {
    return notificationID_;
}

int Notification::getUserID() const {
    return userID_;
}

int Notification::getResourceID() const {
    return resourceID_;
}

std::string Notification::getMessage() const {
    return message_;
}

NotificationType Notification::getType() const {
    return type_;
}

bool Notification::isRead() const {
    return isRead_;
}

std::string Notification::getDateCreated() const {
    return dateCreated_;
}

void Notification::display() const {
    std::string typeStr;
    switch (type_) {
        case NotificationType::BOOK_AVAILABLE:
            typeStr = "BOOK AVAILABLE";
            break;
        case NotificationType::FINE_APPLIED:
            typeStr = "FINE APPLIED";
            break;
        case NotificationType::RETURN_CONFIRMED:
            typeStr = "RETURN CONFIRMED";
            break;
        case NotificationType::BORROW_APPROVED:
            typeStr = "BORROW APPROVED";
            break;
    }

    std::cout << "[" << (isRead_ ? " " : "*") << "] " << typeStr << " - " << dateCreated_ << "\n";
    std::cout << "  " << message_ << "\n";
}

std::string Notification::serialize() const {
    std::string typeStr;
    switch (type_) {
        case NotificationType::BOOK_AVAILABLE: typeStr = "0"; break;
        case NotificationType::FINE_APPLIED: typeStr = "1"; break;
        case NotificationType::RETURN_CONFIRMED: typeStr = "2"; break;
        case NotificationType::BORROW_APPROVED: typeStr = "3"; break;
    }

    std::ostringstream oss;
    oss << notificationID_ << "|" << userID_ << "|" << resourceID_ << "|"
        << message_ << "|" << typeStr << "|" << (isRead_ ? "1" : "0") << "|" << dateCreated_;
    return oss.str();
}

Notification* Notification::deserialize(const std::string& line) {
    std::istringstream iss(line);
    std::string idStr, userIDStr, resourceIDStr, message, typeStr, readStr, dateStr;

    std::getline(iss, idStr, '|');
    std::getline(iss, userIDStr, '|');
    std::getline(iss, resourceIDStr, '|');
    std::getline(iss, message, '|');
    std::getline(iss, typeStr, '|');
    std::getline(iss, readStr, '|');
    std::getline(iss, dateStr, '|');

    int id = std::stoi(idStr);
    int userID = std::stoi(userIDStr);
    int resourceID = std::stoi(resourceIDStr);
    NotificationType type = static_cast<NotificationType>(std::stoi(typeStr));
    bool read = (readStr == "1");

    Notification* notif = new Notification(id, userID, resourceID, message, type, dateStr);
    notif->setRead(read);
    return notif;
}
