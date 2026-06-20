#ifndef LIBRARYMANAGER_H
#define LIBRARYMANAGER_H

#include <vector>
#include <string>
#include <map>
#include <set>
#include "Resource.h"
#include "Usermanager.h"
#include "BorrowRecord.h"
#include "ReportGenerator.h"
#include "Notification.h"
#include "Validator.h"
#include "BookRating.h"
#include "BorrowExtension.h"

using namespace std;

class LibraryManager {
private:
    vector<Resource*> resources_;
    vector<BorrowRecord*> records_;
    map<int, vector<int>> waitlist_; // resourceID -> vector of userIDs waiting
    map<int, int> reservations_; // resourceID -> userID reservation holder
    map<int, vector<Notification*>> notifications_; // userID -> notifications
    map<int, BookRating*> ratings_; // resourceID -> BookRating
    map<int, ExtensionManager*> extensionManagers_; // resourceID -> ExtensionManager
    
    string resourceFilename_;
    string recordFilename_;
    string notificationFilename_;
    string waitlistFilename_;
    string reservationFilename_;
    int nextNotificationID_;

    UserManager& userManager_;
    ReportGenerator& reportGen_;

    void loadResources();
    void saveResources() const;
    void loadRecords();
    void saveRecords() const;
    void loadNotifications();
    void saveNotifications() const;
    void loadWaitlist();
    void saveWaitlist() const;
    void loadReservations();
    void saveReservations() const;
    void rebuildBorrowCounts() const;
    void seedSampleResources();
    void addSeedResource(Resource* resource);
    int generateRecordID() const;
    int generateNotificationID();
    string toLower(string s) const;
    bool contains(string full, string part) const;

public:
    BorrowRecord* findRecord(int userID, int resourceID) const;
    LibraryManager(UserManager& um, ReportGenerator& rg);

    // Resource management
    void addResource(Resource* res);
    void removeResource(int id);
    void displayAllResources() const;
    void displayAvailableResources() const;
    void saveResourcesToFile() const;
    void displayAllMembersBorrowHistory() const;
    void displayIssuedOrOverdueResourceReport() const;

// Category browsing
    set<string> getAllCategories() const;
    vector<string> getCategories() const;
    void displayCategories() const;
    void displayResourcesByCategory(const string& category) const;

    // Macro category browsing (8 categories)
    vector<string> getMacroCategories() const;
    void displayMacroCategories() const;
    void displayResourcesByMacroCategory(const string& macroCategory) const;

    // Search
    Resource* searchByID(int id) const;
    vector<Resource*> searchByTitle(const string& title) const;
    vector<Resource*> searchByCategory(const string& category) const;

    // Waitlist management
    void addToWaitlist(int userID, int resourceID);
    void removeFromWaitlist(int userID, int resourceID);
    bool isInWaitlist(int userID, int resourceID) const;
    vector<int> getWaitlist(int resourceID) const;
    int getWaitlistPosition(int userID, int resourceID) const;

    // Reservation management
    void reserveResource(int userID, int resourceID);
    void cancelReservation(int resourceID);
    bool isReserved(int resourceID) const;
    int getReservationHolder(int resourceID) const;

    // Rating management
    void addRating(int resourceID, int userID, int rating, const string& reviewText = "");
    void requestExtension(int userID, int resourceID, const string& currentDueDate, int daysToExtend);

    // Notification management
    void sendNotification(int userID, int resourceID, const string& message, NotificationType type);
    vector<Notification*> getUserNotifications(int userID) const;
    int getUnreadNotificationCount(int userID) const;
    void markNotificationAsRead(int userID, int notificationID);
    void clearNotifications(int userID);
    void displayAllNotifications() const;
    void displayUserNotifications(int userID) const;

    // Borrowing with waitlist support
    void borrowResource(int userID, int resourceID,
                        const string& issueDate,
                        const string& dueDate);

    void returnResource(int userID, int resourceID,
                        const string& returnDate);

    void applyDamageFee(int userID, int resourceID, double damageFee);

    // History & reports
    void viewBorrowHistory(int userID) const;
    void generateUserReport(int userID);
    void generateResourceReport(int resourceID);

    // For statistics display
    vector<pair<int, string>> getPendingRequests() const;
    const vector<Resource*>& getResources() const { return resources_; }
    vector<BorrowRecord*> getActiveBorrows() const;

    ~LibraryManager();
};

#endif
