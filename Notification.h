#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <string>

enum class NotificationType {
    BOOK_AVAILABLE,
    FINE_APPLIED,
    RETURN_CONFIRMED,
    BORROW_APPROVED
};

class Notification {
private:
    int notificationID_;
    int userID_;
    int resourceID_;
    std::string message_;
    NotificationType type_;
    bool isRead_;
    std::string dateCreated_;

public:
    Notification();
    Notification(int id, int userID, int resourceID,
                  const std::string& message, NotificationType type,
                  const std::string& dateCreated);

    void setRead(bool read);

    int getNotificationID() const;
    int getUserID() const;
    int getResourceID() const;
    std::string getMessage() const;
    NotificationType getType() const;
    bool isRead() const;
    std::string getDateCreated() const;

    void display() const;

    std::string serialize() const;
    static Notification* deserialize(const std::string& line);
};

#endif
