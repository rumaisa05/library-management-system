#include "LibraryManager.h"
#include "Book.h"
#include "Ebook.h"
#include "Member.h"
#include "SYSTEMEXCEPTIONS.h"
#include "Validator.h"
#include "InputHelper.h"
#include <fstream>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <cctype>

using namespace std;

// ---------------- HELPERS ----------------

namespace {
struct BookSeed {
    int id;
    const char* title;
    const char* author;
    const char* category;
    const char* isbn;
    const char* publisher;
    int pages;
};

struct EBookSeed {
    int id;
    const char* title;
    const char* author;
    const char* category;
    float fileSize;
    const char* format;
    const char* link;
};
}

void LibraryManager::addSeedResource(Resource* resource)
{
    if (!resource)
        throw ValidationException("Cannot seed a null resource.");

    resources_.push_back(resource);
}

void LibraryManager::seedSampleResources()
{
    if (!resources_.empty())
        return;

    const BookSeed books[] = {
        {1, "Clean Code", "Robert Martin", "Software Engineering", "9780132350884", "Prentice Hall", 464},
        {2, "The Pragmatic Programmer", "Andrew Hunt", "Software Engineering", "9780135957059", "Addison Wesley", 352},
        {3, "Design Patterns", "Erich Gamma", "Software Engineering", "9780201633610", "Addison Wesley", 395},
        {4, "Refactoring", "Martin Fowler", "Software Engineering", "9780201485677", "Addison Wesley", 448},
        {5, "Code Complete", "Steve McConnell", "Software Engineering", "9780735619678", "Microsoft Press", 960},
        {6, "Introduction to Algorithms", "Thomas Cormen", "Computer Science", "9780262033848", "MIT Press", 1312},
        {7, "Computer Networks", "Andrew Tanenbaum", "Networking", "9780132126953", "Pearson", 960},
        {8, "Operating System Concepts", "Abraham Silberschatz", "Operating Systems", "9781118063330", "Wiley", 976},
        {9, "Database System Concepts", "Abraham Silberschatz", "Databases", "9780073523323", "McGraw Hill", 1376},
        {10, "Artificial Intelligence", "Stuart Russell", "Artificial Intelligence", "9780136042594", "Pearson", 1152},
        {11, "Deep Learning", "Ian Goodfellow", "Artificial Intelligence", "9780262035613", "MIT Press", 800},
        {12, "Python Crash Course", "Eric Matthes", "Programming", "9781593279288", "No Starch", 544},
        {13, "Effective Modern C Plus Plus", "Scott Meyers", "Programming", "9781491903995", "OReilly", 334},
        {14, "C Plus Plus Primer", "Stanley Lippman", "Programming", "9780321714114", "Pearson", 976},
        {15, "Head First Java", "Kathy Sierra", "Programming", "9780596009205", "OReilly", 720},
        {16, "JavaScript The Good Parts", "Douglas Crockford", "Programming", "9780596517748", "OReilly", 176},
        {17, "Eloquent JavaScript", "Marijn Haverbeke", "Programming", "9781593279509", "No Starch", 472},
        {18, "Learning SQL", "Alan Beaulieu", "Databases", "9780596520830", "OReilly", 384},
        {19, "Compilers Principles Techniques and Tools", "Alfred Aho", "Computer Science", "9780321486813", "Pearson", 1000},
        {20, "Modern Operating Systems", "Andrew Tanenbaum", "Operating Systems", "9780133591620", "Pearson", 1136},
        {21, "The Mythical Man Month", "Frederick Brooks", "Project Management", "9780201835953", "Addison Wesley", 336},
        {22, "Peopleware", "Tom DeMarco", "Project Management", "9780321934116", "Addison Wesley", 264},
        {23, "Cracking the Coding Interview", "Gayle McDowell", "Interview Preparation", "9780984782857", "CareerCup", 706},
        {24, "Grokking Algorithms", "Aditya Bhargava", "Computer Science", "9781617292231", "Manning", 256},
        {25, "Computer Organization and Design", "David Patterson", "Computer Architecture", "9780128201091", "Morgan Kaufmann", 800}
    };

    const EBookSeed ebooks[] = {
        {26, "You Dont Know JS Yet", "Kyle Simpson", "Programming", 5.4f, "PDF", "https://example.com/ebooks/you-dont-know-js-yet"},
        {27, "Fluent Python", "Luciano Ramalho", "Programming", 8.2f, "EPUB", "https://example.com/ebooks/fluent-python"},
        {28, "The Linux Command Line", "William Shotts", "Operating Systems", 6.1f, "PDF", "https://example.com/ebooks/the-linux-command-line"},
        {29, "Hands On Machine Learning", "Aurelien Geron", "Artificial Intelligence", 9.7f, "PDF", "https://example.com/ebooks/hands-on-machine-learning"},
        {30, "System Design Interview", "Alex Xu", "Software Engineering", 4.8f, "PDF", "https://example.com/ebooks/system-design-interview"},
        {31, "Machine Learning Yearning", "Andrew Ng", "Artificial Intelligence", 2.3f, "PDF", "https://example.com/ebooks/machine-learning-yearning"},
        {32, "Think Python", "Allen Downey", "Programming", 3.5f, "PDF", "https://example.com/ebooks/think-python"},
        {33, "Automate the Boring Stuff", "Al Sweigart", "Programming", 7.1f, "PDF", "https://example.com/ebooks/automate-the-boring-stuff"},
        {34, "Learning React", "Alex Banks", "Web Development", 4.4f, "EPUB", "https://example.com/ebooks/learning-react"},
        {35, "HTML and CSS", "Jon Duckett", "Web Development", 6.9f, "PDF", "https://example.com/ebooks/html-and-css"},
        {36, "The DevOps Handbook", "Gene Kim", "DevOps", 7.7f, "PDF", "https://example.com/ebooks/the-devops-handbook"},
        {37, "Site Reliability Engineering", "Betsy Beyer", "DevOps", 8.0f, "PDF", "https://example.com/ebooks/site-reliability-engineering"},
        {38, "Docker Deep Dive", "Nigel Poulton", "DevOps", 3.6f, "EPUB", "https://example.com/ebooks/docker-deep-dive"},
        {39, "Kubernetes Up and Running", "Kelsey Hightower", "DevOps", 5.2f, "PDF", "https://example.com/ebooks/kubernetes-up-and-running"},
        {40, "Data Intensive Applications", "Martin Kleppmann", "Databases", 6.5f, "PDF", "https://example.com/ebooks/data-intensive-applications"},
        {41, "Computer Security", "William Stallings", "Cybersecurity", 9.3f, "PDF", "https://example.com/ebooks/computer-security"},
        {42, "Web Application Security", "Andrew Hoffman", "Cybersecurity", 3.9f, "PDF", "https://example.com/ebooks/web-application-security"},
        {43, "Practical Malware Analysis", "Michael Sikorski", "Cybersecurity", 8.6f, "PDF", "https://example.com/ebooks/practical-malware-analysis"},
        {44, "Fundamentals of Database Systems", "Ramez Elmasri", "Databases", 7.2f, "EPUB", "https://example.com/ebooks/fundamentals-of-database-systems"},
        {45, "Computer Graphics", "Donald Hearn", "Computer Graphics", 5.8f, "PDF", "https://example.com/ebooks/computer-graphics"},
        {46, "Game Programming Patterns", "Robert Nystrom", "Programming", 4.1f, "PDF", "https://example.com/ebooks/game-programming-patterns"},
        {47, "Networking All in One", "Doug Lowe", "Networking", 6.3f, "PDF", "https://example.com/ebooks/networking-all-in-one"},
        {48, "Cloud Computing Concepts", "Thomas Erl", "Cloud Computing", 4.7f, "PDF", "https://example.com/ebooks/cloud-computing-concepts"},
        {49, "Practical Statistics for Data Scientists", "Peter Bruce", "Data Science", 3.4f, "EPUB", "https://example.com/ebooks/practical-statistics-for-data-scientists"},
        {50, "Natural Language Processing", "Jacob Eisenstein", "Data Science", 6.0f, "PDF", "https://example.com/ebooks/natural-language-processing"}
    };

    for (const auto& book : books)
        addSeedResource(new Book(book.id, book.title, book.author, book.category,
                                 true, book.isbn, book.publisher, book.pages));

    for (const auto& ebook : ebooks)
        addSeedResource(new EBook(ebook.id, ebook.title, ebook.author, ebook.category,
                                  true, ebook.fileSize, ebook.format, ebook.link));

    saveResources();
}

void LibraryManager::loadResources()
{
    ifstream file(resourceFilename_);
    if (!file.is_open())
        return;

    for (auto* resource : resources_)
        delete resource;
    resources_.clear();

    string line;
    while (getline(file, line)) {
        if (line.empty())
            continue;

        try {
            stringstream ss(line);
            string idToken;
            string typeToken;
            getline(ss, idToken, '|');
            getline(ss, typeToken, '|');

            if (typeToken == "Book")
                resources_.push_back(Book::deserialize(line));
            else if (typeToken == "EBook")
                resources_.push_back(EBook::deserialize(line));
        }
        catch (const exception&) {
        }
    }
}

void LibraryManager::saveResources() const
{
    ofstream file(resourceFilename_);
    if (!file.is_open())
        throw runtime_error("Unable to open resource file for saving.");

    for (const auto* resource : resources_)
        file << resource->serialize() << '\n';
}

void LibraryManager::loadRecords()
{
    ifstream file(recordFilename_);
    if (!file.is_open())
        return;

    for (auto* record : records_)
        delete record;
    records_.clear();

    string line;
    while (getline(file, line)) {
        if (line.empty())
            continue;

        try {
            records_.push_back(BorrowRecord::deserialize(line));
        }
        catch (const exception&) {
        }
    }
}

void LibraryManager::saveRecords() const
{
    ofstream file(recordFilename_);
    if (!file.is_open())
        throw runtime_error("Unable to open borrow record file for saving.");

    for (const auto* record : records_)
        file << record->serialize() << '\n';
}

void LibraryManager::loadNotifications()
{
    ifstream file(notificationFilename_);
    if (!file.is_open())
        return;

    for (const auto& entry : notifications_) {
        for (auto* notif : entry.second)
            delete notif;
    }
    notifications_.clear();
    nextNotificationID_ = 1;

    string line;
    while (getline(file, line)) {
        if (line.empty())
            continue;

        try {
            Notification* notif = Notification::deserialize(line);
            if (!notif)
                continue;

            int id = notif->getNotificationID();
            nextNotificationID_ = max(nextNotificationID_, id + 1);
            notifications_[notif->getUserID()].push_back(notif);

            User* user = userManager_.findUserByID(notif->getUserID());
            Member* member = dynamic_cast<Member*>(user);
            if (member) {
                member->addNotification(notif);
            }
        }
        catch (const exception&) {
        }
    }
}

void LibraryManager::saveNotifications() const
{
    ofstream file(notificationFilename_);
    if (!file.is_open())
        throw runtime_error("Unable to open notification file for saving.");

    for (const auto& entry : notifications_) {
        for (const auto* notif : entry.second) {
            file << notif->serialize() << '\n';
        }
    }
}

void LibraryManager::loadWaitlist()
{
    ifstream file(waitlistFilename_);
    if (!file.is_open())
        return;

    waitlist_.clear();

    string line;
    while (getline(file, line)) {
        if (line.empty())
            continue;

        try {
            stringstream ss(line);
            string resourceIdToken;
            getline(ss, resourceIdToken, '|');
            int resourceID = stoi(resourceIdToken);

            string userList;
            getline(ss, userList, '|');
            vector<int> userIDs;
            stringstream userStream(userList);
            string userToken;
            while (getline(userStream, userToken, ',')) {
                if (!userToken.empty()) {
                    userIDs.push_back(stoi(userToken));
                }
            }

            if (!userIDs.empty())
                waitlist_[resourceID] = move(userIDs);
        }
        catch (const exception&) {
        }
    }
}

void LibraryManager::saveWaitlist() const
{
    ofstream file(waitlistFilename_);
    if (!file.is_open())
        throw runtime_error("Unable to open waitlist file for saving.");

    for (const auto& entry : waitlist_) {
        file << entry.first << "|";
        for (size_t i = 0; i < entry.second.size(); ++i) {
            if (i > 0)
                file << ",";
            file << entry.second[i];
        }
        file << '\n';
    }
}

void LibraryManager::rebuildBorrowCounts() const
{
    for (auto* resource : resources_) {
        if (resource)
            resource->setAvailability(true);
    }

    for (auto* user : userManager_.getUsers()) {
        Member* member = dynamic_cast<Member*>(user);
        if (!member)
            continue;
        member->resetBorrowCount();
        member->removeAllBorrowedBooks();
    }

    for (const auto* record : records_) {
        if (!record || !record->getReturnDate().empty())
            continue;

        Member* member = dynamic_cast<Member*>(userManager_.findUserByID(record->getUserID()));
        if (member) {
            member->incrementBorrow();
            member->addBorrowedBook(record->getResourceID(),
                                    record->getIssueDate(),
                                    record->getDueDate());
        }

        Resource* resource = searchByID(record->getResourceID());
        if (resource)
            resource->setAvailability(false);
    }

    saveResources();
    userManager_.saveToFile();
}

int LibraryManager::generateRecordID() const {
    int maxRecordID = 0;

    for (const auto* record : records_) {
        if (record && record->getRecordID() > maxRecordID)
            maxRecordID = record->getRecordID();
    }

    return maxRecordID + 1;
}

int LibraryManager::generateNotificationID() {
    return nextNotificationID_++;
}

// ---------------- CATEGORY BROWSING ----------------

set<string> LibraryManager::getAllCategories() const {
    set<string> categories;
    for (const auto* res : resources_) {
        categories.insert(res->getCategory());
    }
    return categories;
}

vector<string> LibraryManager::getCategories() const {
    set<string> cats = getAllCategories();
    return vector<string>(cats.begin(), cats.end());
}

void LibraryManager::displayCategories() const {
    set<string> categories = getAllCategories();
    
    if (categories.empty()) {
        cout << "No categories available.\n";
        return;
    }

    cout << "\n╔═════════════════════════════════┐\n";
    cout << "║   BROWSE ALL CATEGORIES            ║\n";
    cout << "╚═════════════════════════════════╝\n";
    int index = 1;
    for (const auto& cat : categories) {
        cout << "[" << index << "] " << cat << "\n";
        index++;
    }
    cout << "===================================\n";
}

void LibraryManager::displayResourcesByCategory(const string& category) const {
    vector<Resource*> results = searchByCategory(category);
    
    if (results.empty()) {
        cout << "No resources found in category: " << category << "\n";
        return;
    }

    cout << "\n=========== " << category << " ===========\n";
    for (size_t i = 0; i < results.size(); i++) {
        cout << "\n[" << (i + 1) << "] ";
        results[i]->displayDetails();
        cout << "----------------------\n";
    }
}

// ---------------- MACRO CATEGORY BROWSING ----------------

vector<string> LibraryManager::getMacroCategories() const {
    set<string> macroCategories;
    
    for (const auto* res : resources_) {
        string macroCat = Validator::getMacroCategory(res->getCategory());
        macroCategories.insert(macroCat);
    }
    
    return vector<string>(macroCategories.begin(), macroCategories.end());
}

void LibraryManager::displayMacroCategories() const {
    vector<string> categories = getMacroCategories();
    
    if (categories.empty()) {
        cout << "No categories available.\n";
        return;
    }

    cout << "\n╔════════════════════════════════════╗\n";
    cout << "║    ALL MACRO CATEGORIES (8)        ║\n";
    cout << "╚════════════════════════════════════╝\n";
    int index = 1;
    for (const auto& cat : categories) {
        cout << "[" << index << "] " << cat << "\n";
        index++;
    }
    cout << "==========================================\n";
}

void LibraryManager::displayResourcesByMacroCategory(const string& macroCategory) const {
    vector<Resource*> results;
    
    for (const auto* res : resources_) {
        string macroCat = Validator::getMacroCategory(res->getCategory());
        if (macroCat == macroCategory) {
            results.push_back(const_cast<Resource*>(res));
        }
    }
    
    if (results.empty()) {
        cout << "No resources found in macro category: " << macroCategory << "\n";
        return;
    }

    cout << "\n=========== " << macroCategory << " ===========\n";
    for (size_t i = 0; i < results.size(); i++) {
        cout << "\n[" << (i + 1) << "] ";
        results[i]->displayDetails();
        cout << "----------------------\n";
    }
}

// ---------------- WAITLIST MANAGEMENT ----------------

void LibraryManager::addToWaitlist(int userID, int resourceID) {
    Validator::validateID(userID);
    Validator::validateID(resourceID);

    if (!userManager_.findMemberByID(userID))
        throw NotFoundException("Only registered members can join a waitlist.");

    if (!searchByID(resourceID))
        throw NotFoundException("Resource not found.");

    auto& list = waitlist_[resourceID];
    for (int uid : list) {
        if (uid == userID)
            return; // Already in waitlist
    }
    list.push_back(userID);
    saveWaitlist();
}

void LibraryManager::removeFromWaitlist(int userID, int resourceID) {
    auto it = waitlist_.find(resourceID);
    if (it == waitlist_.end())
        return;

    auto& vec = it->second;
    for (auto vit = vec.begin(); vit != vec.end(); ++vit) {
        if (*vit == userID) {
            vec.erase(vit);
            saveWaitlist();
            return;
        }
    }
}

bool LibraryManager::isInWaitlist(int userID, int resourceID) const {
    auto it = waitlist_.find(resourceID);
    if (it == waitlist_.end())
        return false;

    for (int uid : it->second) {
        if (uid == userID)
            return true;
    }
    return false;
}

vector<int> LibraryManager::getWaitlist(int resourceID) const {
    auto it = waitlist_.find(resourceID);
    if (it == waitlist_.end())
        return vector<int>();
    return it->second;
}

int LibraryManager::getWaitlistPosition(int userID, int resourceID) const {
    auto it = waitlist_.find(resourceID);
    if (it == waitlist_.end())
        return -1;

    int pos = 1;
    for (int uid : it->second) {
        if (uid == userID)
            return pos;
        pos++;
    }
    return -1;
}

void LibraryManager::loadReservations()
{
    reservations_.clear();
    ifstream file(reservationFilename_);
    if (!file.is_open())
        return;

    string line;
    while (getline(file, line)) {
        if (line.empty())
            continue;

        int resourceID = 0;
        int userID = 0;
        char delimiter;
        istringstream iss(line);
        if (iss >> resourceID >> delimiter >> userID && delimiter == '|') {
            reservations_[resourceID] = userID;
        }
    }
}

void LibraryManager::saveReservations() const
{
    ofstream file(reservationFilename_);
    if (!file.is_open())
        throw runtime_error("Unable to open reservations file for saving.");

    for (const auto& entry : reservations_) {
        file << entry.first << "|" << entry.second << "\n";
    }
}

bool LibraryManager::isReserved(int resourceID) const
{
    return reservations_.find(resourceID) != reservations_.end();
}

int LibraryManager::getReservationHolder(int resourceID) const
{
    auto it = reservations_.find(resourceID);
    return it == reservations_.end() ? -1 : it->second;
}

void LibraryManager::cancelReservation(int resourceID)
{
    auto it = reservations_.find(resourceID);
    if (it != reservations_.end()) {
        reservations_.erase(it);
        saveReservations();
    }
}

void LibraryManager::reserveResource(int userID, int resourceID)
{
    Validator::validateID(userID);
    Validator::validateID(resourceID);

    Member* member = userManager_.findMemberByID(userID);
    if (!member)
        throw AuthorizationException("Only members can reserve resources.");

    Resource* res = searchByID(resourceID);
    if (!res)
        throw NotFoundException("Resource not found.");

    BorrowRecord* rec = findRecord(userID, resourceID);
    if (rec && rec->getReturnDate().empty())
        throw ResourceStateException("You have already borrowed this resource.");

    if (isReserved(resourceID)) {
        if (getReservationHolder(resourceID) == userID)
            throw ResourceStateException("You have already reserved this resource.");
        throw ResourceStateException("Resource is already reserved by another member.");
    }

    if (res->isDigital() && !res->isPhysical())
        throw ResourceStateException("Digital resources cannot be reserved. Please request online access instead.");

    if (!res->checkAvailability())
        throw ResourceStateException("Resource is currently unavailable. Please use the waitlist instead.");

    reservations_[resourceID] = userID;
    saveReservations();

    string message = "You have reserved Resource ID " + to_string(resourceID) + " (" + res->getTitle() + ") for later pickup.";
    sendNotification(userID, resourceID, message, NotificationType::BOOK_AVAILABLE);
}

vector<pair<int, string>> LibraryManager::getPendingRequests() const {
    vector<pair<int, string>> requests;
    int requestID = 1;

    for (const auto& entry : waitlist_) {
        const int resourceID = entry.first;
        Resource* resource = searchByID(resourceID);
        const string title = resource ? resource->getTitle() : "Unknown Resource";

        for (size_t index = 0; index < entry.second.size(); ++index) {
            const int userID = entry.second[index];
            const User* user = userManager_.findUserByID(userID);

            string userLabel = "User ID " + to_string(userID);
            if (user) {
                userLabel += " (" + user->getFirstName() + " " + user->getLastName() + ")";
            }

            string description = "Waitlist #" + to_string(index + 1) +
                                 " | " + userLabel +
                                 " | Resource ID " + to_string(resourceID) +
                                 " - " + title;
            requests.push_back({requestID++, description});
        }
    }

    return requests;
}

// ---------------- NOTIFICATION MANAGEMENT ----------------

void LibraryManager::sendNotification(int userID, int resourceID, const string& message, NotificationType type) {
    // Get current date
    time_t now = time(nullptr);
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&now));
    string dateStr(buf);

    Notification* notif = new Notification(
        generateNotificationID(),
        userID,
        resourceID,
        message,
        type,
        dateStr
    );

    notifications_[userID].push_back(notif);

    User* user = userManager_.findUserByID(userID);
    Member* member = dynamic_cast<Member*>(user);
    if (member) {
        member->addNotification(notif);
    }

    saveNotifications();
}

vector<Notification*> LibraryManager::getUserNotifications(int userID) const {
    auto it = notifications_.find(userID);
    if (it == notifications_.end())
        return vector<Notification*>();
    return it->second;
}

int LibraryManager::getUnreadNotificationCount(int userID) const {
    auto it = notifications_.find(userID);
    if (it == notifications_.end())
        return 0;

    int count = 0;
    for (const auto* notif : it->second) {
        if (!notif->isRead())
            count++;
    }
    return count;
}

void LibraryManager::markNotificationAsRead(int userID, int notificationID) {
    auto it = notifications_.find(userID);
    if (it == notifications_.end())
        return;

    for (auto* notif : it->second) {
        if (notif->getNotificationID() == notificationID) {
            notif->setRead(true);
            User* user = userManager_.findUserByID(userID);
            Member* member = dynamic_cast<Member*>(user);
            if (member) {
                member->markNotificationAsRead(notificationID);
            }
            saveNotifications();
            return;
        }
    }
}

void LibraryManager::clearNotifications(int userID) {
    auto it = notifications_.find(userID);
    if (it == notifications_.end())
        return;

    for (auto* notif : it->second) {
        delete notif;
    }
    notifications_.erase(it);
}

void LibraryManager::displayAllNotifications() const {
    if (notifications_.empty()) {
        cout << "No notifications have been generated yet.\n";
        return;
    }

    cout << "\n===== ALL NOTIFICATIONS =====\n";
    for (const auto& entry : notifications_) {
        int userID = entry.first;
        for (const auto* notif : entry.second) {
            cout << "User ID " << userID << " | ";
            notif->display();
            cout << "----------------------\n";
        }
    }
}

void LibraryManager::displayUserNotifications(int userID) const {
    auto it = notifications_.find(userID);
    if (it == notifications_.end() || it->second.empty()) {
        cout << "No notifications found for user ID " << userID << ".\n";
        return;
    }

    cout << "\n===== NOTIFICATIONS FOR USER " << userID << " =====\n";
    for (const auto* notif : it->second) {
        notif->display();
        cout << "----------------------\n";
    }
}

BorrowRecord* LibraryManager::findRecord(int userID, int resourceID) const {
    BorrowRecord* latestReturned = nullptr;

    for (auto it = records_.rbegin(); it != records_.rend(); ++it) {
        BorrowRecord* record = *it;
        if (!record)
            continue;

        if (record->getUserID() == userID && record->getResourceID() == resourceID) {
            if (record->getReturnDate().empty())
                return record;

            if (!latestReturned)
                latestReturned = record;
        }
    }

    return latestReturned;
}

string LibraryManager::toLower(string s) const {
    for (char &c : s)
        c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    return s;
}

bool LibraryManager::contains(string full, string part) const {
    // Make searches case-insensitive so users do not have to match exact title/category casing.
    return toLower(full).find(toLower(part)) != string::npos;
}

// ---------------- CONSTRUCTOR ----------------

LibraryManager::LibraryManager(UserManager& um, ReportGenerator& rg)
    : resourceFilename_("resources.txt"), recordFilename_("borrow_records.txt"),
      notificationFilename_("notifications.txt"), waitlistFilename_("waitlist.txt"),
      reservationFilename_("reservations.txt"), nextNotificationID_(1), userManager_(um), reportGen_(rg) {
    loadResources();
    if (resources_.empty())
        seedSampleResources();

    loadRecords();
    loadNotifications();
    loadWaitlist();
    loadReservations();
    rebuildBorrowCounts();
}

// ---------------- ADD RESOURCE ----------------

void LibraryManager::addResource(Resource* res) {
    if (!res)
        throw ValidationException("Cannot add null resource.");

    resources_.push_back(res);
    saveResources();
    cout << "\nResource added successfully!\n";
    cout << "Resource ID: " << res->getResourceID() << "\n\n";
}

// ---------------- REMOVE RESOURCE (SAFE FIX) ----------------

void LibraryManager::removeResource(int id) {
    Validator::validateID(id);

    for (int i = 0; i < (int)resources_.size(); i++) {
        if (resources_[i]->getResourceID() == id) {

            if (!resources_[i]->checkAvailability())
                throw ResourceStateException("Cannot delete a borrowed resource.");

            delete resources_[i];
            resources_.erase(resources_.begin() + i);

            saveResources();
            cout << "\nResource removed successfully!\n\n";
            return;
        }
    }

    throw NotFoundException("Resource not found.");
}

// ---------------- DISPLAY ----------------

void LibraryManager::displayAllResources() const {
    if (resources_.empty()) {
        cout << "No resources available.\n";
        return;
    }

    for (Resource* res : resources_) {
        res->displayDetails();
        cout << "----------------------\n";
    }
}

void LibraryManager::displayAvailableResources() const {
    vector<Resource*> available;
    for (auto* res : resources_) {
        if (res && res->checkAvailability() && !isReserved(res->getResourceID()))
            available.push_back(res);
    }

    if (available.empty()) {
        cout << "No available resources currently.\n";
        return;
    }

    cout << "\n=========== AVAILABLE RESOURCES ===========\n";
    for (auto* res : available) {
        res->displayDetails();
        cout << "----------------------\n";
    }
}

void LibraryManager::saveResourcesToFile() const {
    saveResources();
}

void LibraryManager::displayAllMembersBorrowHistory() const {
    bool foundAny = false;

    for (auto* user : userManager_.getUsers()) {
        const Member* member = dynamic_cast<const Member*>(user);
        if (!member)
            continue;

        foundAny = true;
        cout << "\n===== MEMBER BORROWING SUMMARY =====\n";
        cout << "Member: " << member->getFirstName() << " " << member->getLastName() << " (ID " << member->getUserID() << ")\n";
        cout << "Current Borrows: " << member->getBorrowedCount() << "\n";

        if (member->getBorrowedCount() > 0) {
            cout << "--- Current Borrowed Resources ---\n";
            for (const auto& book : member->getBorrowedBooks()) {
                cout << "Resource ID: " << book.resourceID
                     << " | Issue: " << book.issueDate
                     << " | Due: " << book.dueDate;
                if (book.isOverdue) {
                    cout << " | OVERDUE";
                }
                cout << "\n";
            }
        }

        cout << "\n--- Full Borrow History ---\n";
        viewBorrowHistory(member->getUserID());
    }

    if (!foundAny)
        cout << "No members found for borrowing history report.\n";
}

void LibraryManager::displayIssuedOrOverdueResourceReport() const {
    bool foundAny = false;
    time_t now = time(nullptr);

    auto parseDateToTime = [](const string& date) {
        int year = 0, month = 0, day = 0;
        sscanf(date.c_str(), "%d-%d-%d", &year, &month, &day);
        tm tmDate = {};
        tmDate.tm_year = year - 1900;
        tmDate.tm_mon = month - 1;
        tmDate.tm_mday = day;
        tmDate.tm_hour = 12;
        return mktime(&tmDate);
    };

    cout << "\n===== ISSUED AND OVERDUE RESOURCES =====\n";
    for (auto* rec : records_) {
        if (!rec)
            continue;

        if (!rec->getReturnDate().empty())
            continue;

        foundAny = true;
        Resource* res = searchByID(rec->getResourceID());
        User* user = userManager_.findUserByID(rec->getUserID());
        bool isEbook = (res && dynamic_cast<EBook*>(res));

        time_t dueTime = parseDateToTime(rec->getDueDate());
        string status = rec->getStatus();
        bool isOverdue = (status == "Overdue");
        int overdueDays = 0;

        if (isOverdue) {
            overdueDays = static_cast<int>(difftime(now, dueTime) / (24 * 60 * 60));
        }

        cout << "Record ID   : " << rec->getRecordID() << "\n";
        cout << "Resource ID : " << rec->getResourceID();
        if (res) cout << " (" << res->getTitle() << ")";
        cout << "\n";
        if (user)
            cout << "Borrowed By : " << user->getFirstName() << " " << user->getLastName() << " (ID " << user->getUserID() << ")\n";
        cout << "Issue Date  : " << rec->getIssueDate() << "\n";
        cout << "Due Date    : " << rec->getDueDate() << "\n";
        cout << "Status      : " << status << "\n";
        if (isOverdue) {
            cout << "Overdue Days: " << overdueDays;
            if (!isEbook)
                cout << " | Fine Estimate: Rs. " << (overdueDays * 10.0);
            else
                cout << " | Fine Estimate: Rs. 0 (eBook)";
            cout << "\n";
        }
        cout << "Current Fine: Rs. " << rec->getFineAmount() << "\n";
        cout << "---------------------------\n";
    }

    if (!foundAny)
        cout << "No currently issued or overdue resources found.\n";
}

// ---------------- SEARCH ----------------

Resource* LibraryManager::searchByID(int id) const {
    Validator::validateID(id);

    for (auto res : resources_)
        if (res->getResourceID() == id)
            return res;

    return nullptr;
}

vector<Resource*> LibraryManager::searchByTitle(const string& title) const {
    vector<Resource*> results;

    for (auto res : resources_)
        if (contains(res->getTitle(), title))
            results.push_back(res);

    return results;
}

vector<Resource*> LibraryManager::searchByCategory(const string& category) const {
    vector<Resource*> results;

    for (auto res : resources_)
        if (contains(res->getCategory(), category))
            results.push_back(res);

    return results;
}

// ---------------- BORROW (HARDENED) ----------------

void LibraryManager::borrowResource(int userID, int resourceID,
                                    const string& issueDate,
                                    const string& dueDate)
{
    Validator::validateID(userID);
    Validator::validateID(resourceID);
    Validator::validateDate(issueDate);
    Validator::validateDate(dueDate);

    User* user = userManager_.findUserByID(userID);
    if (!user)
        throw NotFoundException("User not found.");

    Member* member = dynamic_cast<Member*>(user);
    if (!member)
        throw AuthorizationException("Only members can borrow resources.");

    // Check if member is locked
    if (member->isLocked()) {
        throw AuthorizationException("Account is locked. Please try again later or contact support.");
    }

    // Check membership validity
    if (!member->isMembershipValid()) {
        throw AuthorizationException("Membership has expired. Please renew to borrow books.");
    }

    // Track borrow count on the member so the current borrow limit can be enforced.
    if (!member->canBorrow())
        throw ResourceStateException(("Borrow limit reached! Your " + member->getMembershipTypeName() + 
            " membership allows " + to_string(member->getMaxBorrowLimit()) + " books. "
            "Return some books to borrow more.").c_str());

    Resource* res = searchByID(resourceID);
    if (!res)
        throw NotFoundException("Resource not found.");

    BorrowRecord* existingRecord = findRecord(userID, resourceID);
    if (existingRecord && existingRecord->getReturnDate().empty())
        throw ResourceStateException("This member has already borrowed this resource.");

    if (isReserved(resourceID) && getReservationHolder(resourceID) != userID)
        throw ResourceStateException("Resource is reserved by another member.");

    if (!res->checkAvailability())
        throw ResourceStateException(res->isDigital() ? "No digital license is available right now." : "Resource already borrowed.");

    // If a member has outstanding fines, borrowing is blocked until the balance is cleared.
    if (member->getBalance() < 0) {
        throw ResourceStateException("You have outstanding fines of Rs. " + to_string(-member->getBalance()) + ". Please pay your fines before borrowing additional resources.");
    }

    if (isReserved(resourceID) && getReservationHolder(resourceID) == userID)
        cancelReservation(resourceID);

    if (auto* ebook = dynamic_cast<EBook*>(res)) {
        ebook->borrowResource();
    } else {
        res->borrowResource();
    }

    BorrowRecord* record = new BorrowRecord();
    record->createRecord(resourceID, generateRecordID(), userID, issueDate, dueDate, res->isDigital());

    records_.push_back(record);
    member->incrementBorrow();
    member->addBorrowedBook(resourceID, issueDate, dueDate);
    removeFromWaitlist(userID, resourceID);

// Send confirmation notification
    string message = "You have successfully borrowed '" + res->getTitle() + 
                    "'. Due date: " + dueDate + ". Please return on time to avoid late fees.";
    sendNotification(userID, resourceID, message, NotificationType::BORROW_APPROVED);

    saveResources();
    saveRecords();
    userManager_.saveToFile();
    cout << "Borrow successful!\n";
    cout << "   Book: " << res->getTitle() << "\n";
    cout << "   Due Date: " << dueDate << "\n";
    cout << "   Late Fee: Rs. 10 per day after due date\n";
}

// ---------------- RETURN (IMPROVED LOGIC) ----------------

void LibraryManager::applyDamageFee(int userID, int resourceID, double damageFee)
{
    Validator::validateID(userID);
    Validator::validateID(resourceID);

    if (damageFee <= 0.0)
        throw ValidationException("Damage fee must be greater than zero.");

    Member* member = userManager_.findMemberByID(userID);
    if (!member)
        throw NotFoundException("Only registered members can be charged a damage fee.");

    member->updateBalance(-damageFee);
    userManager_.saveToFile();

    string message = "A damage fee of Rs. " + to_string(damageFee) +
                     " has been charged for Resource ID " + to_string(resourceID) + ".";
    sendNotification(userID, resourceID, message, NotificationType::FINE_APPLIED);
}

void LibraryManager::returnResource(int userID, int resourceID,
                                    const string& returnDate)
{
    Validator::validateID(userID);
    Validator::validateID(resourceID);
    Validator::validateDate(returnDate);

    BorrowRecord* rec = nullptr;

    for (auto it = records_.rbegin(); it != records_.rend(); ++it) {
        BorrowRecord* record = *it;
        if (record && record->getUserID() == userID &&
            record->getResourceID() == resourceID &&
            record->getReturnDate().empty()) {
            rec = record;
            break;
        }
    }

    if (!rec)
        throw NotFoundException("Active borrow record not found.");

    // Calculate late fee before updating return date
    double lateFee = 0.0;
    string dueDate = rec->getDueDate();
    
    // Calculate days overdue
    int year, month, day;
    sscanf(dueDate.c_str(), "%d-%d-%d", &year, &month, &day);
    tm due = {};
    due.tm_year = year - 1900;
    due.tm_mon = month - 1;
    due.tm_mday = day;
    time_t dueTime = mktime(&due);
    
    sscanf(returnDate.c_str(), "%d-%d-%d", &year, &month, &day);
    tm ret = {};
    ret.tm_year = year - 1900;
    ret.tm_mon = month - 1;
    ret.tm_mday = day;
    time_t retTime = mktime(&ret);
    
    double daysOverdue = difftime(retTime, dueTime) / (24 * 60 * 60);
    Resource* res = searchByID(resourceID);
    if (daysOverdue > 0 && !(res && dynamic_cast<EBook*>(res))) {
        lateFee = daysOverdue * 10.0; // Rs. 10 per day
    }

    rec->updateReturnDate(returnDate);

    if (res) {
        if (auto* ebook = dynamic_cast<EBook*>(res)) {
            ebook->returnResource();
        } else if (!res->checkAvailability()) {
            res->returnResource();
        } else {
            res->setAvailability(true);
        }
    }

    User* user = userManager_.findUserByID(userID);
    Member* member = dynamic_cast<Member*>(user);
    // Only members track borrow counts, so admins or invalid users are ignored here.
    if (member) {
        member->decrementBorrow();
        // Also remove from Member's borrowedBooks_ list to keep in sync
        member->removeBorrowedBook(resourceID);
    }
    
    // Display return summary
    cout << "\nRETURN SUMMARY\n";
    cout << "--------------\n";
    if (res) {
        cout << "Book: " << res->getTitle() << "\n";
    }
    cout << "Borrowed Date: " << rec->getIssueDate() << "\n";
    cout << "Due Date: " << dueDate << "\n";
    cout << "Returned Date: " << returnDate << "\n";
    
    if (lateFee > 0) {
        cout << "LATE FEE: Rs. " << lateFee << " (" << (int)daysOverdue << " days overdue)\n";

        if (member) {
            member->updateBalance(-lateFee);

            string fineMessage = "A late fine of Rs. " + to_string(lateFee) +
                                 " was applied for Resource ID " + to_string(resourceID) + ".";
            sendNotification(userID, resourceID, fineMessage, NotificationType::FINE_APPLIED);

            if (member->getBalance() >= 0) {
                cout << "Fine settled using account credit. Remaining balance: Rs. " << member->getBalance() << "\n";
            } else {
                cout << "Outstanding fine: Rs. " << -member->getBalance() << ". Please pay at the library counter.\n";
            }
        }
    } else {
        cout << "Returned on time! No late fees.\n";
    }

    // Send return confirmation notification
    if (member) {
        string returnMessage = "Return confirmed for Resource ID " + to_string(resourceID);
        if (res)
            returnMessage += " (" + res->getTitle() + ")";
        returnMessage += ".";
        sendNotification(userID, resourceID, returnMessage, NotificationType::RETURN_CONFIRMED);
    }

    // Check waitlist and notify first person if any
    auto waitlistIt = waitlist_.find(resourceID);
    if (waitlistIt != waitlist_.end() && !waitlistIt->second.empty()) {
        int nextUserID = waitlistIt->second.front();
        waitlistIt->second.erase(waitlistIt->second.begin());
        
        // Get resource title for notification
        string resourceTitle = "";
        if (res) {
            resourceTitle = res->getTitle();
        }
        
        // Send notification to the next person
        string message = "Good news! Resource ID " + to_string(resourceID) + 
                        (resourceTitle.empty() ? "" : " (" + resourceTitle + ")") + 
                        " that you were waiting for is now available. You can borrow it now.";
        sendNotification(nextUserID, resourceID, message, NotificationType::BOOK_AVAILABLE);
        
        cout << "\n>>> NOTIFICATION SENT <<<\n";
        cout << "User ID " << nextUserID << " has been notified that the book is available.\n";
    }

    saveResources();
    saveRecords();
    saveWaitlist();
    userManager_.saveToFile();
    cout << "Return successful!\n";
}

// ---------------- HISTORY ----------------

void LibraryManager::viewBorrowHistory(int userID) const {
    Validator::validateID(userID);

    bool found = false;

    for (auto rec : records_) {
        if (rec->getUserID() == userID) {
            Resource* res = searchByID(rec->getResourceID());
            cout << "\n=========== BORROW RECORD ===========\n";
            if (res) {
                cout << "Title       : " << res->getTitle() << endl;
                cout << "Author      : " << res->getAuthor() << endl;
                cout << "Category    : " << res->getCategory() << endl;
            } else {
                cout << "Title       : Unknown\n";
            }
            cout << "Record ID   : " << rec->getRecordID() << endl;
            cout << "Resource ID : " << rec->getResourceID() << endl;
            cout << "Issue Date  : " << rec->getIssueDate() << endl;
            cout << "Due Date    : " << rec->getDueDate() << endl;
            cout << "Return Date : " << (rec->getReturnDate().empty() ? string("Not returned") : rec->getReturnDate()) << endl;
            cout << "Status      : " << rec->getStatus() << endl;
            cout << "Fine        : " << rec->getFineAmount() << endl;
            found = true;
        }
    }

    if (!found)
        cout << "No history found.\n";
}

// ---------------- REPORTS ----------------

void LibraryManager::generateUserReport(int userID) {
    Validator::validateID(userID);

    vector<BorrowRecord> temp;

    for (auto r : records_)
        temp.push_back(*r);

    cout << reportGen_.generateUserReport(userID, temp);
}

void LibraryManager::generateResourceReport(int resourceID) {
    Validator::validateID(resourceID);

    vector<BorrowRecord> temp;

    for (auto r : records_)
        temp.push_back(*r);

    cout << reportGen_.generateResourceReport(resourceID, temp);
}

// ---------------- DESTRUCTOR ----------------

vector<BorrowRecord*> LibraryManager::getActiveBorrows() const {
    vector<BorrowRecord*> active;
    for (auto rec : records_) {
        if (rec && rec->getReturnDate().empty()) {
            active.push_back(rec);
        }
    }
    return active;
}

// ---------------- RATING MANAGEMENT ----------------

void LibraryManager::addRating(int resourceID, int userID, int rating, const string& reviewText) {
    Validator::validateID(resourceID);
    Validator::validateID(userID);

    if (rating < 1 || rating > 5) {
        throw ValidationException("Rating must be between 1 and 5.");
    }

    User* user = userManager_.findUserByID(userID);
    if (!user) {
        throw NotFoundException("User not found.");
    }

    string username = user->getUsername();

    if (ratings_.find(resourceID) == ratings_.end()) {
        ratings_[resourceID] = new BookRating(resourceID);
    }

    ratings_[resourceID]->addRating(userID, username, rating, reviewText);
}

// ---------------- EXTENSION MANAGEMENT ----------------

void LibraryManager::requestExtension(int userID, int resourceID, const string& currentDueDate, int daysToExtend) {
    Validator::validateID(userID);
    Validator::validateID(resourceID);

    if (daysToExtend < 1 || daysToExtend > 14) {
        throw ValidationException("Extension days must be between 1 and 14.");
    }

    if (extensionManagers_.find(resourceID) == extensionManagers_.end()) {
        extensionManagers_[resourceID] = new ExtensionManager(resourceID);
    }

    extensionManagers_[resourceID]->requestExtension(userID, resourceID, currentDueDate, daysToExtend);
}

LibraryManager::~LibraryManager() {
    try {
        saveResources();
        saveRecords();
        saveNotifications();
        saveWaitlist();
        saveReservations();
    }
    catch (...) {
    }

    for (auto r : records_) delete r;
    for (auto r : resources_) delete r;

    for (auto& entry : notifications_) {
        for (auto* notif : entry.second)
            delete notif;
    }

    for (auto& entry : ratings_) {
        delete entry.second;
    }

    for (auto& entry : extensionManagers_) {
        delete entry.second;
    }

    records_.clear();
    resources_.clear();
}
