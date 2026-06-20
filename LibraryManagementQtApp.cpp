#include <QApplication>
#include <QMainWindow>
#include <QStackedWidget>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QGroupBox>
#include <QComboBox>
#include <QSpinBox>
#include <QMessageBox>
#include <QInputDialog>
#include <QTextEdit>
#include <QFile>
#include <QTextStream>
#include <QDate>
#include <QDialog>
#include <QHeaderView>
#include <QCheckBox>
#include <QDesktopServices>
#include <QUrl>
#include <QFont>
#include <QGridLayout>
#include <QTabWidget>

#include <algorithm>

static QString sanitizeField(const QString& text) {
    QString safe = text;
    return safe.replace('|', ' ');
}

static QString dateToString(const QDate& date) {
    return date.toString("yyyy-MM-dd");
}

static QDate stringToDate(const QString& text) {
    QDate date = QDate::fromString(text, "yyyy-MM-dd");
    if (!date.isValid())
        date = QDate::currentDate();
    return date;
}

struct Resource {
    int id = 0;
    QString type;
    QString title;
    QString author;
    QString category;
    bool available = true;
    bool digital = false;
    QString isbn;
    QString publisher;
    int pages = 0;
    float fileSize = 0.0f;
    QString format;
    QString downloadLink;
    int licenseCount = 0;
    QString location;

    QString serialize() const {
        return QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11|%12|%13|%14|%15")
            .arg(id)
            .arg(type)
            .arg(sanitizeField(title))
            .arg(sanitizeField(author))
            .arg(sanitizeField(category))
            .arg(available ? 1 : 0)
            .arg(digital ? 1 : 0)
            .arg(sanitizeField(isbn))
            .arg(sanitizeField(publisher))
            .arg(pages)
            .arg(fileSize)
            .arg(sanitizeField(format))
            .arg(sanitizeField(downloadLink))
            .arg(licenseCount)
            .arg(sanitizeField(location));
    }

    static Resource deserialize(const QString& line) {
        Resource res;
        QStringList parts = line.split('|');
        if (parts.size() < 15)
            return res;
        res.id = parts[0].toInt();
        res.type = parts[1];
        res.title = parts[2];
        res.author = parts[3];
        res.category = parts[4];
        res.available = parts[5].toInt() != 0;
        res.digital = parts[6].toInt() != 0;
        res.isbn = parts[7];
        res.publisher = parts[8];
        res.pages = parts[9].toInt();
        res.fileSize = parts[10].toFloat();
        res.format = parts[11];
        res.downloadLink = parts[12];
        res.licenseCount = parts[13].toInt();
        res.location = parts[14];
        return res;
    }

    bool isAvailable() const {
        if (digital)
            return licenseCount > 0;
        return available;
    }

    QString displayType() const {
        return type.isEmpty() ? (digital ? "EBook" : "Book") : type;
    }

    QString availabilityText() const {
        if (!isAvailable())
            return "Not Available";
        return digital ? QString("%1 licenses").arg(licenseCount) : "Available";
    }
};

struct User {
    int id = 0;
    QString username;
    QString password;
    QString role = "Member";
    bool active = true;
    double balance = 0.0;
    QString membershipType = "Silver";
    QDate membershipExpiry = QDate::currentDate().addYears(1);
    QString firstName;
    QString lastName;
    QString email;
    QString phone;
    QString address;

    QString serialize() const {
        return QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11|%12|%13|%14|%15|%16")
            .arg(id)
            .arg(sanitizeField(username))
            .arg(sanitizeField(password))
            .arg(role)
            .arg(active ? 1 : 0)
            .arg(balance)
            .arg(membershipType)
            .arg(dateToString(membershipExpiry))
            .arg(sanitizeField(firstName))
            .arg(sanitizeField(lastName))
            .arg(sanitizeField(email))
            .arg(sanitizeField(phone))
            .arg(sanitizeField(address))
            .arg(sanitizeField(QString()))
            .arg(sanitizeField(QString()))
            .arg(sanitizeField(QString()));
    }

    static User deserialize(const QString& line) {
        User user;
        QStringList parts = line.split('|');
        if (parts.size() < 13)
            return user;
        user.id = parts[0].toInt();
        user.username = parts[1];
        user.password = parts[2];
        user.role = parts[3];
        user.active = parts[4].toInt() != 0;
        user.balance = parts[5].toDouble();
        user.membershipType = parts[6].isEmpty() ? "Silver" : parts[6];
        user.membershipExpiry = stringToDate(parts[7]);
        user.firstName = parts[8];
        user.lastName = parts[9];
        user.email = parts[10];
        user.phone = parts[11];
        user.address = parts[12];
        return user;
    }

    int borrowLimit() const {
        if (membershipType == "Gold") return 5;
        if (membershipType == "Platinum") return 10;
        return 2;
    }

    int borrowDays() const {
        if (membershipType == "Gold") return 21;
        if (membershipType == "Platinum") return 30;
        return 14;
    }

    bool membershipValid() const {
        return QDate::currentDate() <= membershipExpiry;
    }
};

struct BorrowRecord {
    int recordId = 0;
    int userId = 0;
    int resourceId = 0;
    QString issueDate;
    QString dueDate;
    QString returnDate;
    bool digital = false;
    double fine = 0.0;

    QString serialize() const {
        return QString("%1|%2|%3|%4|%5|%6|%7|%8")
            .arg(recordId)
            .arg(userId)
            .arg(resourceId)
            .arg(issueDate)
            .arg(dueDate)
            .arg(returnDate)
            .arg(digital ? 1 : 0)
            .arg(fine);
    }

    static BorrowRecord deserialize(const QString& line) {
        BorrowRecord record;
        QStringList parts = line.split('|');
        if (parts.size() < 8)
            return record;
        record.recordId = parts[0].toInt();
        record.userId = parts[1].toInt();
        record.resourceId = parts[2].toInt();
        record.issueDate = parts[3];
        record.dueDate = parts[4];
        record.returnDate = parts[5];
        record.digital = parts[6].toInt() != 0;
        record.fine = parts[7].toDouble();
        return record;
    }

    bool isReturned() const {
        return !returnDate.isEmpty();
    }

    double calculateFine() const {
        if (digital)
            return 0.0;
        QDate end = isReturned() ? stringToDate(returnDate) : QDate::currentDate();
        QDate due = stringToDate(dueDate);
        int daysLate = due.daysTo(end);
        if (daysLate <= 0)
            return 0.0;
        return daysLate * 10.0;
    }
};

struct Notification {
    int id = 0;
    int userId = 0;
    QString message;
    bool read = false;
    QString date;

    QString serialize() const {
        return QString("%1|%2|%3|%4|%5")
            .arg(id)
            .arg(userId)
            .arg(sanitizeField(message))
            .arg(read ? 1 : 0)
            .arg(date);
    }

    static Notification deserialize(const QString& line) {
        Notification notif;
        QStringList parts = line.split('|');
        if (parts.size() < 5)
            return notif;
        notif.id = parts[0].toInt();
        notif.userId = parts[1].toInt();
        notif.message = parts[2];
        notif.read = parts[3].toInt() != 0;
        notif.date = parts[4];
        return notif;
    }
};

enum class ExtensionStatus {
    Pending = 0,
    Approved = 1,
    Denied = 2
};

struct Reservation {
    int resourceId = 0;
    int userId = 0;
    QString serialize() const {
        return QString("%1|%2").arg(resourceId).arg(userId);
    }
    static Reservation deserialize(const QString& line) {
        Reservation res;
        QStringList parts = line.split('|');
        if (parts.size() < 2)
            return res;
        res.resourceId = parts[0].toInt();
        res.userId = parts[1].toInt();
        return res;
    }
};

struct WaitlistItem {
    int resourceId = 0;
    int userId = 0;
    QString serialize() const {
        return QString("%1|%2").arg(resourceId).arg(userId);
    }
    static WaitlistItem deserialize(const QString& line) {
        WaitlistItem item;
        QStringList parts = line.split('|');
        if (parts.size() < 2)
            return item;
        item.resourceId = parts[0].toInt();
        item.userId = parts[1].toInt();
        return item;
    }
};

struct RatingEntry {
    int id = 0;
    int resourceId = 0;
    int userId = 0;
    int stars = 0;
    QString review;
    QString date;
    QString serialize() const {
        return QString("%1|%2|%3|%4|%5|%6")
            .arg(id)
            .arg(resourceId)
            .arg(userId)
            .arg(stars)
            .arg(sanitizeField(review))
            .arg(date);
    }
    static RatingEntry deserialize(const QString& line) {
        RatingEntry entry;
        QStringList parts = line.split('|');
        if (parts.size() < 6)
            return entry;
        entry.id = parts[0].toInt();
        entry.resourceId = parts[1].toInt();
        entry.userId = parts[2].toInt();
        entry.stars = parts[3].toInt();
        entry.review = parts[4];
        entry.date = parts[5];
        return entry;
    }
};

struct ExtensionRequest {
    int requestId = 0;
    int userId = 0;
    int resourceId = 0;
    QString currentDueDate;
    int daysRequested = 0;
    ExtensionStatus status = ExtensionStatus::Pending;
    QString requestDate;
    QString responseDate;
    QString adminNotes;
    QString newDueDate;
    QString serialize() const {
        return QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10")
            .arg(requestId)
            .arg(userId)
            .arg(resourceId)
            .arg(currentDueDate)
            .arg(daysRequested)
            .arg(static_cast<int>(status))
            .arg(requestDate)
            .arg(responseDate)
            .arg(sanitizeField(adminNotes))
            .arg(newDueDate);
    }
    static ExtensionRequest deserialize(const QString& line) {
        ExtensionRequest req;
        QStringList parts = line.split('|');
        if (parts.size() < 10)
            return req;
        req.requestId = parts[0].toInt();
        req.userId = parts[1].toInt();
        req.resourceId = parts[2].toInt();
        req.currentDueDate = parts[3];
        req.daysRequested = parts[4].toInt();
        req.status = static_cast<ExtensionStatus>(parts[5].toInt());
        req.requestDate = parts[6];
        req.responseDate = parts[7];
        req.adminNotes = parts[8];
        req.newDueDate = parts[9];
        return req;
    }
};

class DataStore {
public:
    QVector<User> users;
    QVector<Resource> resources;
    QVector<BorrowRecord> records;
    QVector<Notification> notifications;
    QVector<Reservation> reservations;
    QVector<WaitlistItem> waitlist;
    QVector<RatingEntry> ratings;
    QVector<ExtensionRequest> extensions;

    DataStore() {
        loadAll();
    }

    ~DataStore() {
        saveAll();
    }

    void loadAll() {
        loadUsers();
        loadResources();
        loadRecords();
        loadNotifications();
        loadReservations();
        loadWaitlist();
        loadRatings();
        loadExtensions();
        if (resources.isEmpty())
            seedResources();
    }

    void saveAll() {
        saveUsers();
        saveResources();
        saveRecords();
        saveNotifications();
        saveReservations();
        saveWaitlist();
        saveRatings();
        saveExtensions();
    }

    bool hasAdmin() const {
        for (const User& u : users) {
            if (u.role == "Admin" || u.role == "Super Admin")
                return true;
        }
        return false;
    }

    int nextUserId() const {
        int id = 0;
        for (const User& u : users)
            id = std::max(id, u.id);
        return id + 1;
    }

    int nextResourceId() const {
        int id = 0;
        for (const Resource& r : resources)
            id = std::max(id, r.id);
        return id + 1;
    }

    int nextRecordId() const {
        int id = 0;
        for (const BorrowRecord& r : records)
            id = std::max(id, r.recordId);
        return id + 1;
    }

    int nextNotificationId() const {
        int id = 0;
        for (const Notification& n : notifications)
            id = std::max(id, n.id);
        return id + 1;
    }

    int nextRatingId() const {
        int id = 0;
        for (const RatingEntry& r : ratings)
            id = std::max(id, r.id);
        return id + 1;
    }

    int nextExtensionId() const {
        int id = 0;
        for (const ExtensionRequest& ext : extensions)
            id = std::max(id, ext.requestId);
        return id + 1;
    }

    void loadUsers() {
        users.clear();
        QFile file("users.dat");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!line.isEmpty())
                users.append(User::deserialize(line));
        }
    }

    void saveUsers() const {
        QFile file("users.dat");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        QTextStream out(&file);
        for (const User& u : users)
            out << u.serialize() << "\n";
    }

    void loadResources() {
        resources.clear();
        QFile file("resources.dat");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!line.isEmpty())
                resources.append(Resource::deserialize(line));
        }
    }

    void saveResources() const {
        QFile file("resources.dat");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        QTextStream out(&file);
        for (const Resource& r : resources)
            out << r.serialize() << "\n";
    }

    void loadRecords() {
        records.clear();
        QFile file("borrows.dat");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!line.isEmpty())
                records.append(BorrowRecord::deserialize(line));
        }
    }

    void saveRecords() const {
        QFile file("borrows.dat");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        QTextStream out(&file);
        for (const BorrowRecord& r : records)
            out << r.serialize() << "\n";
    }

    void loadNotifications() {
        notifications.clear();
        QFile file("notifications.dat");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!line.isEmpty())
                notifications.append(Notification::deserialize(line));
        }
    }

    void saveNotifications() const {
        QFile file("notifications.dat");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        QTextStream out(&file);
        for (const Notification& n : notifications)
            out << n.serialize() << "\n";
    }

    void loadReservations() {
        reservations.clear();
        QFile file("reservations.dat");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!line.isEmpty())
                reservations.append(Reservation::deserialize(line));
        }
    }

    void saveReservations() const {
        QFile file("reservations.dat");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        QTextStream out(&file);
        for (const Reservation& r : reservations)
            out << r.serialize() << "\n";
    }

    void loadWaitlist() {
        waitlist.clear();
        QFile file("waitlist.dat");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!line.isEmpty())
                waitlist.append(WaitlistItem::deserialize(line));
        }
    }

    void saveWaitlist() const {
        QFile file("waitlist.dat");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        QTextStream out(&file);
        for (const WaitlistItem& item : waitlist)
            out << item.serialize() << "\n";
    }

    void loadRatings() {
        ratings.clear();
        QFile file("ratings.dat");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!line.isEmpty())
                ratings.append(RatingEntry::deserialize(line));
        }
    }

    void saveRatings() const {
        QFile file("ratings.dat");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        QTextStream out(&file);
        for (const RatingEntry& r : ratings)
            out << r.serialize() << "\n";
    }

    void loadExtensions() {
        extensions.clear();
        QFile file("extensions.dat");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!line.isEmpty())
                extensions.append(ExtensionRequest::deserialize(line));
        }
    }

    void saveExtensions() const {
        QFile file("extensions.dat");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        QTextStream out(&file);
        for (const ExtensionRequest& ext : extensions)
            out << ext.serialize() << "\n";
    }

    void createDefaultAdmin() {
        User admin;
        admin.id = nextUserId();
        admin.username = "admin";
        admin.password = "admin123";
        admin.role = "Admin";
        admin.firstName = "Super";
        admin.lastName = "Admin";
        admin.email = "admin@library.local";
        admin.phone = "03000000000";
        admin.address = "Library Headquarters";
        admin.balance = 0.0;
        users.append(admin);
        saveUsers();
    }

    void seedResources() {
        QVector<Resource> seeds = {
            {1, "Book", "Clean Code", "Robert C. Martin", "Software Engineering", true, false, "9780132350884", "Prentice Hall", 464, 0.0f, "", "", 0, "Shelf A1"},
            {2, "Book", "The Pragmatic Programmer", "Andrew Hunt", "Software Engineering", true, false, "9780135957059", "Addison Wesley", 352, 0.0f, "", "", 0, "Shelf A1"},
            {3, "EBook", "You Dont Know JS Yet", "Kyle Simpson", "Programming", true, true, "", "", 0, 5.4f, "PDF", "https://example.com/you-dont-know-js-yet", 5, "Online"},
            {4, "EBook", "Fluent Python", "Luciano Ramalho", "Programming", true, true, "", "", 0, 8.2f, "EPUB", "https://example.com/fluent-python", 3, "Online"},
            {5, "Book", "Data Structures and Algorithms", "K. Mehta", "Computer Science", true, false, "9780195679201", "Oxford", 700, 0.0f, "", "", 0, "Shelf B2"}
        };
        resources = seeds;
        saveResources();
    }
};

class LibraryManager {
public:
    DataStore store;

    LibraryManager() = default;

    User* login(const QString& username, const QString& password) {
        for (auto& u : store.users) {
            if (u.username == username && u.password == password && u.active)
                return &u;
        }
        return nullptr;
    }

    bool usernameExists(const QString& username) const {
        return std::any_of(store.users.begin(), store.users.end(), [&](const User& u) {
            return u.username == username;
        });
    }

    bool hasAdmin() const {
        for (const User& u : store.users) {
            if (u.role == "Admin" || u.role == "Super Admin")
                return true;
        }
        return false;
    }

    bool registerMember(const QString& username, const QString& password,
                        const QString& firstName, const QString& lastName,
                        const QString& email, const QString& phone,
                        const QString& address, QString& error) {
        if (username.trimmed().isEmpty() || password.isEmpty()) {
            error = "Username and password are required.";
            return false;
        }
        if (usernameExists(username)) {
            error = "That username is already taken.";
            return false;
        }
        User user;
        user.id = store.nextUserId();
        user.username = username;
        user.password = password;
        user.role = "Member";
        user.active = true;
        user.balance = 0.0;
        user.membershipType = "Silver";
        user.membershipExpiry = QDate::currentDate().addYears(1);
        user.firstName = firstName;
        user.lastName = lastName;
        user.email = email;
        user.phone = phone;
        user.address = address;
        store.users.append(user);
        store.saveUsers();
        return true;
    }

    bool registerAdmin(const QString& username, const QString& password,
                       const QString& firstName, const QString& lastName,
                       const QString& email, const QString& phone,
                       const QString& address, const QString& accessCode,
                       QString& error) {
        if (username.trimmed().isEmpty() || password.isEmpty()) {
            error = "Username and password are required.";
            return false;
        }
        if (usernameExists(username)) {
            error = "That username is already taken.";
            return false;
        }

        bool createSuperAdmin = !hasAdmin();
        if (!createSuperAdmin) {
            // Verify admin access code for creating normal admins
            if (accessCode != "ADMIN123") {
                error = "Invalid admin access code.";
                return false;
            }
        }

        User admin;
        admin.id = store.nextUserId();
        admin.username = username;
        admin.password = password;
        admin.role = createSuperAdmin ? "Super Admin" : "Admin";
        admin.active = true;
        admin.balance = 0.0;
        admin.firstName = firstName;
        admin.lastName = lastName;
        admin.email = email;
        admin.phone = phone;
        admin.address = address;
        store.users.append(admin);
        store.saveUsers();
        return true;
    }

    Resource* resourceById(int resourceId) {
        for (auto& res : store.resources) {
            if (res.id == resourceId)
                return &res;
        }
        return nullptr;
    }

    User* userById(int userId) {
        for (auto& u : store.users) {
            if (u.id == userId)
                return &u;
        }
        return nullptr;
    }

    QVector<BorrowRecord*> activeBorrowRecords(int userId) {
        QVector<BorrowRecord*> active;
        for (auto& r : store.records) {
            if (r.userId == userId && !r.isReturned())
                active.append(&r);
        }
        return active;
    }

    QVector<BorrowRecord*> activeBorrowRecordsForResource(int resourceId) {
        QVector<BorrowRecord*> active;
        for (auto& r : store.records) {
            if (r.resourceId == resourceId && !r.isReturned())
                active.append(&r);
        }
        return active;
    }

    int activeBorrowCount(int userId) const {
        int count = 0;
        for (const auto& r : store.records) {
            if (r.userId == userId && !r.isReturned())
                count++;
        }
        return count;
    }

    bool canBorrow(const User& user, const Resource& resource, QString& error) {
        if (!user.active) {
            error = "Your account is suspended.";
            return false;
        }
        if (user.role != "Member") {
            error = "Only members can borrow resources.";
            return false;
        }
        if (!user.membershipValid()) {
            error = "Your membership has expired. Renew it before borrowing.";
            return false;
        }
        if (user.balance < 0.0) {
            error = "You have outstanding fines. Please pay them before borrowing.";
            return false;
        }
        if (!resource.isAvailable()) {
            error = "That resource is currently unavailable.";
            return false;
        }
        if (activeBorrowCount(user.id) >= user.borrowLimit()) {
            error = QString("Borrow limit reached. Your current membership allows %1 borrows.").arg(user.borrowLimit());
            return false;
        }
        return true;
    }

    QString borrowResource(int userId, int resourceId) {
        User* user = userById(userId);
        Resource* resource = resourceById(resourceId);
        if (!user || !resource)
            return "Invalid user or resource.";

        QString error;
        if (!canBorrow(*user, *resource, error))
            return error;

        if (resource->digital) {
            resource->licenseCount = std::max(0, resource->licenseCount - 1);
        } else {
            resource->available = false;
        }

        BorrowRecord record;
        record.recordId = store.nextRecordId();
        record.userId = user->id;
        record.resourceId = resource->id;
        record.issueDate = dateToString(QDate::currentDate());
        record.dueDate = dateToString(QDate::currentDate().addDays(user->borrowDays()));
        record.digital = resource->digital;
        record.returnDate = "";
        record.fine = 0.0;
        store.records.append(record);
        store.saveRecords();
        store.saveResources();

        QString message = QString("You borrowed '%1'. Due date: %2.")
            .arg(resource->title, record.dueDate);
        sendNotification(user->id, resource->id, message);
        return QString();
    }

    QString returnResource(int userId, int resourceId) {
        User* user = userById(userId);
        Resource* resource = resourceById(resourceId);
        if (!user || !resource)
            return "Invalid user or resource.";

        BorrowRecord* matched = nullptr;
        for (auto& record : store.records) {
            if (record.userId == userId && record.resourceId == resourceId && !record.isReturned()) {
                matched = &record;
                break;
            }
        }
        if (!matched)
            return "No active borrow record found for that resource.";

        matched->returnDate = dateToString(QDate::currentDate());
        matched->fine = matched->calculateFine();
        if (!resource->digital)
            resource->available = true;
        else
            resource->licenseCount += 1;

        store.saveRecords();
        store.saveResources();

        if (matched->fine > 0) {
            user->balance -= matched->fine;
            store.saveUsers();
            sendNotification(user->id, resource->id,
                QString("Return processed. Fine charged: Rs. %1.").arg(matched->fine));
            return QString("Return accepted. Fine Rs. %1 applied.").arg(matched->fine);
        }
        sendNotification(user->id, resource->id, "Return processed successfully.");
        return "Return processed successfully.";
    }

    QVector<Resource> searchResources(const QString& query) const {
        QVector<Resource> results;
        QString lower = query.trimmed().toLower();
        for (const auto& res : store.resources) {
            if (res.title.toLower().contains(lower)
                || res.author.toLower().contains(lower)
                || res.category.toLower().contains(lower)
                || res.isbn.toLower().contains(lower)) {
                results.append(res);
            }
        }
        return results;
    }

    QVector<Resource> availableResources() const {
        QVector<Resource> results;
        for (const auto& res : store.resources) {
            if (res.isAvailable())
                results.append(res);
        }
        return results;
    }

    QVector<BorrowRecord> borrowedRecords(int userId) const {
        QVector<BorrowRecord> results;
        for (const auto& r : store.records) {
            if (r.userId == userId)
                results.append(r);
        }
        return results;
    }

    QVector<Notification> notificationsForUser(int userId) const {
        QVector<Notification> results;
        for (const auto& n : store.notifications) {
            if (n.userId == userId)
                results.append(n);
        }
        return results;
    }

    void sendNotification(int userId, int resourceId, const QString& message) {
        Notification notif;
        notif.id = store.nextNotificationId();
        notif.userId = userId;
        notif.message = message;
        notif.read = false;
        notif.date = dateToString(QDate::currentDate());
        store.notifications.append(notif);
        store.saveNotifications();
    }

    void markNotificationRead(int notificationId) {
        for (auto& n : store.notifications) {
            if (n.id == notificationId) {
                n.read = true;
                store.saveNotifications();
                return;
            }
        }
    }

    void renewMembership(User& user, const QString& type, int months) {
        user.membershipType = type;
        QDate base = QDate::currentDate();
        if (user.membershipExpiry > base)
            base = user.membershipExpiry;
        user.membershipExpiry = base.addMonths(months);
        store.saveUsers();
    }

    QString payFine(User& user, double amount) {
        if (amount <= 0.0)
            return "Payment must be greater than zero.";
        user.balance += amount;
        store.saveUsers();
        return QString();
    }

    QString outstandingFine(const User& user) const {
        if (user.balance < 0.0)
            return QString("Rs. %1").arg(-user.balance);
        return "None";
    }

    QVector<User> allMembers() const {
        QVector<User> members;
        for (const auto& u : store.users) {
            if (u.role == "Member")
                members.append(u);
        }
        return members;
    }

    QVector<User> allAdmins() const {
        QVector<User> admins;
        for (const auto& u : store.users) {
            if (u.role == "Admin")
                admins.append(u);
        }
        return admins;
    }

    bool isReserved(int resourceId) const {
        for (const Reservation& r : store.reservations) {
            if (r.resourceId == resourceId)
                return true;
        }
        return false;
    }

    int reservationHolder(int resourceId) const {
        for (const Reservation& r : store.reservations) {
            if (r.resourceId == resourceId)
                return r.userId;
        }
        return -1;
    }

    QString reserveResource(int userId, int resourceId) {
        User* user = userById(userId);
        Resource* resource = resourceById(resourceId);
        if (!user || !resource)
            return "Invalid user or resource.";
        if (user->role != "Member")
            return "Only members can reserve resources.";
        if (resource->digital)
            return "Digital resources cannot be reserved.";
        if (isReserved(resourceId)) {
            if (reservationHolder(resourceId) == userId)
                return "You already reserved this resource.";
            return "Resource is already reserved by another member.";
        }
        if (resource->isAvailable())
            return "Resource is still available. You can borrow it directly.";
        Reservation res;
        res.resourceId = resourceId;
        res.userId = userId;
        store.reservations.append(res);
        store.saveReservations();
        return QString();
    }

    QString cancelReservation(int resourceId) {
        for (int i = 0; i < store.reservations.size(); ++i) {
            if (store.reservations[i].resourceId == resourceId) {
                store.reservations.removeAt(i);
                store.saveReservations();
                return QString();
            }
        }
        return "Reservation not found.";
    }

    bool isInWaitlist(int userId, int resourceId) const {
        for (const auto& item : store.waitlist) {
            if (item.userId == userId && item.resourceId == resourceId)
                return true;
        }
        return false;
    }

    int waitlistPosition(int userId, int resourceId) const {
        int position = 1;
        for (const auto& item : store.waitlist) {
            if (item.resourceId == resourceId) {
                if (item.userId == userId)
                    return position;
                position++;
            }
        }
        return -1;
    }

    QString addToWaitlist(int userId, int resourceId) {
        User* user = userById(userId);
        Resource* resource = resourceById(resourceId);
        if (!user || !resource)
            return "Invalid user or resource.";
        if (user->role != "Member")
            return "Only members can join a waitlist.";
        if (resource->isAvailable())
            return "Resource is available now. Borrow it instead of joining the waitlist.";
        if (isInWaitlist(userId, resourceId))
            return "You are already on the waitlist for this resource.";
        WaitlistItem item;
        item.userId = userId;
        item.resourceId = resourceId;
        store.waitlist.append(item);
        store.saveWaitlist();
        return QString();
    }

    QString requestExtension(int userId, int resourceId, int daysRequested) {
        User* user = userById(userId);
        Resource* resource = resourceById(resourceId);
        if (!user || !resource)
            return "Invalid user or resource.";
        if (user->role != "Member")
            return "Only members can request due date extensions.";
        if (daysRequested < 1 || daysRequested > 14)
            return "Extension days must be between 1 and 14.";
        BorrowRecord* record = nullptr;
        for (auto& rec : store.records) {
            if (rec.userId == userId && rec.resourceId == resourceId && !rec.isReturned()) {
                record = &rec;
                break;
            }
        }
        if (!record)
            return "No active borrow record found for this resource.";
        for (const auto& ext : store.extensions) {
            if (ext.userId == userId && ext.resourceId == resourceId && ext.status == ExtensionStatus::Pending)
                return "You already have a pending extension request for this resource.";
        }
        ExtensionRequest ext;
        ext.requestId = store.nextExtensionId();
        ext.userId = userId;
        ext.resourceId = resourceId;
        ext.currentDueDate = record->dueDate;
        ext.daysRequested = daysRequested;
        ext.status = ExtensionStatus::Pending;
        ext.requestDate = dateToString(QDate::currentDate());
        store.extensions.append(ext);
        store.saveExtensions();
        return QString();
    }

    QVector<ExtensionRequest> pendingExtensions() const {
        QVector<ExtensionRequest> pending;
        for (const auto& ext : store.extensions) {
            if (ext.status == ExtensionStatus::Pending)
                pending.append(ext);
        }
        return pending;
    }

    QVector<ExtensionRequest> extensionsForUser(int userId) const {
        QVector<ExtensionRequest> list;
        for (const auto& ext : store.extensions) {
            if (ext.userId == userId)
                list.append(ext);
        }
        return list;
    }

    QString processExtension(int requestId, bool approve, const QString& notes) {
        int foundIndex = -1;
        for (int i = 0; i < store.extensions.size(); ++i) {
            if (store.extensions[i].requestId == requestId) {
                foundIndex = i;
                break;
            }
        }
        if (foundIndex < 0)
            return "Extension request not found.";
        
        auto& ext = store.extensions[foundIndex];
        if (ext.status != ExtensionStatus::Pending)
            return "This request has already been processed.";
        
        ext.status = approve ? ExtensionStatus::Approved : ExtensionStatus::Denied;
        ext.adminNotes = notes;
        ext.responseDate = dateToString(QDate::currentDate());
        
        if (approve) {
            QDate due = stringToDate(ext.currentDueDate);
            ext.newDueDate = dateToString(due.addDays(ext.daysRequested));
            for (int i = 0; i < store.records.size(); ++i) {
                if (store.records[i].userId == ext.userId && 
                    store.records[i].resourceId == ext.resourceId && 
                    !store.records[i].isReturned()) {
                    store.records[i].dueDate = ext.newDueDate;
                    break;
                }
            }
            store.saveRecords();
        }
        store.saveExtensions();
        return QString();
    }

    QString addRating(int userId, int resourceId, int stars, const QString& review) {
        User* user = userById(userId);
        Resource* resource = resourceById(resourceId);
        if (!user || !resource)
            return "Invalid user or resource.";
        if (user->role != "Member")
            return "Only members can submit ratings.";
        if (stars < 1 || stars > 5)
            return "Rating must be between 1 and 5 stars.";
        if (review.trimmed().isEmpty())
            return "Review text cannot be empty.";
        for (const auto& r : store.ratings) {
            if (r.userId == userId && r.resourceId == resourceId)
                return "You have already reviewed this resource.";
        }
        RatingEntry entry;
        entry.id = store.nextRatingId();
        entry.userId = userId;
        entry.resourceId = resourceId;
        entry.stars = stars;
        entry.review = review.trimmed();
        entry.date = dateToString(QDate::currentDate());
        store.ratings.append(entry);
        store.saveRatings();
        return QString();
    }

    QVector<RatingEntry> ratingsForResource(int resourceId) const {
        QVector<RatingEntry> results;
        for (const auto& r : store.ratings) {
            if (r.resourceId == resourceId)
                results.append(r);
        }
        return results;
    }

    QString suspendMember(int userId) {
        for (auto& u : store.users) {
            if (u.id == userId && u.role == "Member") {
                u.active = false;
                store.saveUsers();
                return QString();
            }
        }
        return "Member not found.";
    }

    QString reinstateMember(int userId) {
        for (auto& u : store.users) {
            if (u.id == userId && u.role == "Member") {
                u.active = true;
                store.saveUsers();
                return QString();
            }
        }
        return "Member not found.";
    }

    QString deleteMember(int userId) {
        for (int i = 0; i < store.users.size(); ++i) {
            if (store.users[i].id == userId && store.users[i].role == "Member") {
                store.users.removeAt(i);
                store.saveUsers();
                return QString();
            }
        }
        return "Member not found.";
    }

    QString addResource(const Resource& resource) {
        if (resource.title.trimmed().isEmpty() || resource.author.trimmed().isEmpty())
            return "Title and author are required.";
        Resource copy = resource;
        copy.id = store.nextResourceId();
        store.resources.append(copy);
        store.saveResources();
        return QString();
    }
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr)
        : QMainWindow(parent), manager(new LibraryManager()) {
        setWindowTitle("Library Management System");
        resize(1000, 700);
        setWindowIcon(QIcon());
        stacked = new QStackedWidget(this);
        setCentralWidget(stacked);

        setStyleSheet(R"(
            * { margin: 0; padding: 0; }
            QMainWindow, QWidget { background-color: #f8fafc; }
            QDialog { background-color: #f8fafc; }
            QLabel {
                color: #1e293b;
                font-size: 12px;
            }
            QLabel[level="title"] {
                color: #0f172a;
                font-size: 24px;
                font-weight: bold;
                margin-bottom: 12px;
            }
            QLabel[level="subtitle"] {
                color: #475569;
                font-size: 14px;
                font-weight: bold;
                margin: 8px 0;
            }
            
            QPushButton {
                background-color: #2563eb;
                color: white;
                border: none;
                border-radius: 6px;
                padding: 10px 18px;
                font-weight: 500;
                font-size: 12px;
                min-height: 36px;
                min-width: 80px;
            }
            QPushButton:hover {
                background-color: #1d4ed8;
                border: 1px solid #1e40af;
            }
            QPushButton:pressed {
                background-color: #1e40af;
            }
            QPushButton:disabled {
                background-color: #cbd5e1;
                color: #94a3b8;
            }
            
            QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox, QTextEdit {
                background-color: white;
                color: #1e293b;
                border: 1px solid #d1d5db;
                border-radius: 6px;
                padding: 8px 12px;
                font-size: 12px;
                selection-background-color: #bfdbfe;
            }
            QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QDoubleSpinBox:focus, QTextEdit:focus {
                border: 2px solid #2563eb;
                background-color: #f0f9ff;
            }
            
            QTableWidget {
                border: 1px solid #e2e8f0;
                border-radius: 6px;
                gridline-color: #e2e8f0;
                background-color: white;
            }
            QTableWidget::item {
                padding: 6px 8px;
                border: none;
                color: #1e293b;
            }
            QTableWidget::item:selected {
                background-color: #dbeafe;
                color: #0f172a;
            }
            QHeaderView::section {
                background-color: #1e293b;
                color: white;
                padding: 8px;
                border: none;
                font-weight: bold;
                font-size: 12px;
            }
            
            QGroupBox {
                color: #0f172a;
                border: 1px solid #e2e8f0;
                border-radius: 8px;
                margin-top: 8px;
                padding-top: 8px;
                background-color: white;
                font-weight: 500;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                subcontrol-position: top left;
                margin-left: 12px;
                padding: 0 4px;
            }
            
            QTabWidget::pane {
                border: 1px solid #e2e8f0;
                background-color: white;
            }
            QTabBar::tab {
                background-color: #f1f5f9;
                color: #475569;
                padding: 8px 16px;
                border: 1px solid #e2e8f0;
                border-radius: 4px 4px 0 0;
                margin-right: 2px;
            }
            QTabBar::tab:selected {
                background-color: #2563eb;
                color: white;
                border-bottom: 2px solid #2563eb;
            }
            
            QToolTip {
                background-color: #1e293b;
                color: white;
                border: none;
                border-radius: 4px;
                padding: 4px 8px;
            }
            
            QCheckBox {
                color: #1e293b;
                spacing: 6px;
            }
            QCheckBox::indicator {
                width: 18px;
                height: 18px;
            }
            QCheckBox::indicator:unchecked {
                background-color: white;
                border: 1px solid #d1d5db;
                border-radius: 4px;
            }
            QCheckBox::indicator:checked {
                background-color: #2563eb;
                border: 1px solid #2563eb;
                color: white;
            }
            
            QDialog {
                background-color: #f8fafc;
            }
            QDialog QTableWidget {
                border-radius: 8px;
                border: 1px solid #e2e8f0;
                background-color: white;
                margin: 0px;
                gridline-color: #e2e8f0;
            }
            QDialog QPushButton {
                min-width: 70px;
            }
            QDialog QLabel {
                color: #1e293b;
            }
        )");;

        createInitPage();
        createLoginPage();
        createRegisterPage();
        createMemberPage();
        createAdminPage();
        stacked->addWidget(initPage);
        stacked->addWidget(loginPage);
        stacked->addWidget(registerPage);
        stacked->addWidget(memberPage);
        stacked->addWidget(adminPage);
        
        // Check if admin exists; if not, show initialization screen
        if (!manager->hasAdmin()) {
            stacked->setCurrentWidget(initPage);
        } else {
            stacked->setCurrentWidget(loginPage);
        }
    }

private slots:
    void onLogin() {
        QString username = loginUsername->text().trimmed();
        QString password = loginPassword->text();
        if (username.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(this, "Login", "Please enter username and password.");
            return;
        }
        User* user = manager->login(username, password);
        if (!user) {
            QMessageBox::warning(this, "Login", "Invalid credentials or account inactive.");
            return;
        }
        currentUser = user;
        if (currentUser->role == "Admin") {
            updateAdminSummary();
            stacked->setCurrentWidget(adminPage);
        } else {
            updateMemberSummary();
            stacked->setCurrentWidget(memberPage);
        }
    }

    void onShowRegister() {
        stacked->setCurrentWidget(registerPage);
    }

    void onRegisterSubmit() {
        QString username = regUsername->text().trimmed();
        QString password = regPassword->text();
        QString firstName = regFirstName->text().trimmed();
        QString lastName = regLastName->text().trimmed();
        QString email = regEmail->text().trimmed();
        QString phone = regPhone->text().trimmed();
        QString address = regAddress->text().trimmed();
        QString error;
        if (!manager->registerMember(username, password, firstName, lastName, email, phone, address, error)) {
            QMessageBox::warning(this, "Register", error);
            return;
        }
        QMessageBox::information(this, "Register", "Member registered successfully. You can now login.");
        stacked->setCurrentWidget(loginPage);
    }

    void onLogout() {
        currentUser = nullptr;
        loginUsername->clear();
        loginPassword->clear();
        stacked->setCurrentWidget(loginPage);
    }

    void onShowAdminRegister(bool superAdminRegistration = false) {
        QDialog dlg(this);
        dlg.setWindowTitle(superAdminRegistration ? "Super Admin Registration" : "Admin Registration");
        dlg.resize(480, 560);
        QVBoxLayout* layout = new QVBoxLayout(&dlg);

        QLabel* header = new QLabel(superAdminRegistration ? "Register as Super Admin" : "Register as Administrator");
        header->setAlignment(Qt::AlignCenter);
        header->setStyleSheet("font-size: 16px; font-weight: bold;");
        layout->addWidget(header);

        QFormLayout* form = new QFormLayout;
        QLineEdit* adminUsername = new QLineEdit;
        QLineEdit* adminPassword = new QLineEdit;
        adminPassword->setEchoMode(QLineEdit::Password);
        QLineEdit* adminFirstName = new QLineEdit;
        QLineEdit* adminLastName = new QLineEdit;
        QLineEdit* adminEmail = new QLineEdit;
        QLineEdit* adminPhone = new QLineEdit;
        QLineEdit* adminAddress = new QLineEdit;
        QLineEdit* adminAccessCode = new QLineEdit;
        adminAccessCode->setEchoMode(QLineEdit::Password);

        form->addRow("Username:", adminUsername);
        form->addRow("Password:", adminPassword);
        form->addRow("First Name:", adminFirstName);
        form->addRow("Last Name:", adminLastName);
        form->addRow("Email:", adminEmail);
        form->addRow("Phone:", adminPhone);
        form->addRow("Address:", adminAddress);
        if (!superAdminRegistration) {
            form->addRow("Admin Access Code:", adminAccessCode);
        }
        layout->addLayout(form);

        if (superAdminRegistration) {
            QLabel* note = new QLabel("This will create the first Super Admin for the system.");
            note->setWordWrap(true);
            layout->addWidget(note);
        }

        QHBoxLayout* buttons = new QHBoxLayout;
        QPushButton* registerBtn = new QPushButton("Register");
        QPushButton* cancelBtn = new QPushButton("Cancel");
        buttons->addStretch();
        buttons->addWidget(registerBtn);
        buttons->addWidget(cancelBtn);
        layout->addLayout(buttons);

        connect(registerBtn, &QPushButton::clicked, [&]() {
            QString username = adminUsername->text().trimmed();
            QString password = adminPassword->text();
            QString firstName = adminFirstName->text().trimmed();
            QString lastName = adminLastName->text().trimmed();
            QString email = adminEmail->text().trimmed();
            QString phone = adminPhone->text().trimmed();
            QString address = adminAddress->text().trimmed();
            QString accessCode = superAdminRegistration ? QString() : adminAccessCode->text();

            if (username.isEmpty() || password.isEmpty() || firstName.isEmpty() || lastName.isEmpty()) {
                QMessageBox::warning(&dlg, "Validation", "Please fill in all required fields.");
                return;
            }

            QString error;
            if (!manager->registerAdmin(username, password, firstName, lastName, email, phone, address, accessCode, error)) {
                QMessageBox::warning(&dlg, "Admin Registration", error);
                return;
            }

            QString successMessage = superAdminRegistration
                ? "Super Admin registered successfully!\n\nYou can now login with your credentials."
                : "Administrator registered successfully!\n\nYou can now login with your credentials.";

            QMessageBox::information(&dlg, "Admin Registration", successMessage);
            dlg.accept();
            stacked->setCurrentWidget(loginPage);
        });

        connect(cancelBtn, &QPushButton::clicked, &dlg, &QDialog::reject);
        dlg.exec();
    }

    void onBrowseResources() {
        QDialog dlg(this);
        dlg.setWindowTitle("Browse & Borrow Resources");
        dlg.resize(920, 580);
        QVBoxLayout* mainLayout = new QVBoxLayout(&dlg);
        mainLayout->setContentsMargins(20, 20, 20, 20);
        mainLayout->setSpacing(16);

        QLabel* title = new QLabel("Search Available Resources");
        title->setStyleSheet("font-size: 14px; font-weight: bold; color: #0f172a;");
        mainLayout->addWidget(title);

        QHBoxLayout* searchLayout = new QHBoxLayout;
        searchLayout->setSpacing(12);
        QLabel* label = new QLabel("Search:");
        label->setMinimumWidth(70);
        QLineEdit* searchInput = new QLineEdit;
        searchInput->setPlaceholderText("Search by title, author, category...");
        QPushButton* searchButton = new QPushButton("Search");
        searchButton->setMinimumWidth(100);
        searchLayout->addWidget(label);
        searchLayout->addWidget(searchInput);
        searchLayout->addWidget(searchButton);
        mainLayout->addLayout(searchLayout);

        QTableWidget* table = new QTableWidget;
        table->setColumnCount(7);
        table->setHorizontalHeaderLabels({"ID", "Title", "Author", "Category", "Type", "Availability", "Location"});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        table->setAlternatingRowColors(true);
        mainLayout->addWidget(table);

        QPushButton* borrowButton = new QPushButton("Borrow Selected");
        borrowButton->setMinimumHeight(38);
        QPushButton* backButton = new QPushButton("Close");
        backButton->setMinimumHeight(38);
        backButton->setStyleSheet(R"(
            QPushButton {
                background-color: #64748b;
            }
            QPushButton:hover {
                background-color: #475569;
            }
        )");
        QHBoxLayout* buttonLayout = new QHBoxLayout;
        buttonLayout->addStretch();
        buttonLayout->addWidget(borrowButton);
        buttonLayout->addWidget(backButton);
        mainLayout->addLayout(buttonLayout);

        auto populate = [&](const QVector<Resource>& resources) {
            table->setRowCount(resources.size());
            for (int row = 0; row < resources.size(); ++row) {
                const Resource& res = resources[row];
                table->setItem(row, 0, new QTableWidgetItem(QString::number(res.id)));
                table->setItem(row, 1, new QTableWidgetItem(res.title));
                table->setItem(row, 2, new QTableWidgetItem(res.author));
                table->setItem(row, 3, new QTableWidgetItem(res.category));
                table->setItem(row, 4, new QTableWidgetItem(res.displayType()));
                table->setItem(row, 5, new QTableWidgetItem(res.availabilityText()));
                table->setItem(row, 6, new QTableWidgetItem(res.location));
            }
        };

        auto updateSearch = [&]() {
            QString query = searchInput->text().trimmed();
            QVector<Resource> results = query.isEmpty() ? manager->availableResources()
                                                        : manager->searchResources(query);
            populate(results);
        };

        updateSearch();
        connect(searchButton, &QPushButton::clicked, updateSearch);
        connect(searchInput, &QLineEdit::returnPressed, updateSearch);

        connect(borrowButton, &QPushButton::clicked, [&]() {
            int row = table->currentRow();
            if (row < 0) {
                QMessageBox::warning(&dlg, "Borrow", "Please select a resource first.");
                return;
            }
            int resourceId = table->item(row, 0)->text().toInt();
            QString message = manager->borrowResource(currentUser->id, resourceId);
            if (!message.isEmpty()) {
                QMessageBox::warning(&dlg, "Borrow", message);
            } else {
                QMessageBox::information(&dlg, "Success", "Resource borrowed successfully!");
                updateSearch();
                updateMemberSummary();
            }
        });

        connect(backButton, &QPushButton::clicked, &dlg, &QDialog::accept);
        dlg.exec();
    }

    void onViewBorrowed() {
        QDialog dlg(this);
        dlg.setWindowTitle("My Borrowed Books");
        dlg.resize(900, 550);
        QVBoxLayout* mainLayout = new QVBoxLayout(&dlg);
        mainLayout->setContentsMargins(20, 20, 20, 20);
        mainLayout->setSpacing(16);

        QLabel* title = new QLabel("Your Borrowed Resources");
        title->setStyleSheet("font-size: 14px; font-weight: bold; color: #0f172a;");
        mainLayout->addWidget(title);

        QTableWidget* table = new QTableWidget;
        table->setColumnCount(7);
        table->setHorizontalHeaderLabels({"Record", "Resource", "Issued", "Due", "Returned", "Status", "Fine"});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        table->setAlternatingRowColors(true);
        mainLayout->addWidget(table);

        QPushButton* returnButton = new QPushButton("↩️ Return Selected");
        returnButton->setMinimumHeight(38);
        QPushButton* closeButton = new QPushButton("Close");
        closeButton->setMinimumHeight(38);
        closeButton->setStyleSheet(R"(
            QPushButton {
                background-color: #64748b;
            }
            QPushButton:hover {
                background-color: #475569;
            }
        )");
        QHBoxLayout* buttons = new QHBoxLayout;
        buttons->addStretch();
        buttons->addWidget(returnButton);
        buttons->addWidget(closeButton);
        mainLayout->addLayout(buttons);

        QVector<BorrowRecord> records = manager->borrowedRecords(currentUser->id);
        table->setRowCount(records.size());
        for (int row = 0; row < records.size(); ++row) {
            const BorrowRecord& rec = records[row];
            Resource* res = manager->resourceById(rec.resourceId);
            QString status = rec.isReturned() ? "Returned" : (stringToDate(rec.dueDate) < QDate::currentDate() ? "Overdue" : "Borrowed");
            table->setItem(row, 0, new QTableWidgetItem(QString::number(rec.recordId)));
            table->setItem(row, 1, new QTableWidgetItem(res ? res->title : "Unknown"));
            table->setItem(row, 2, new QTableWidgetItem(rec.issueDate));
            table->setItem(row, 3, new QTableWidgetItem(rec.dueDate));
            table->setItem(row, 4, new QTableWidgetItem(rec.returnDate.isEmpty() ? "—" : rec.returnDate));
            table->setItem(row, 5, new QTableWidgetItem(status));
            table->setItem(row, 6, new QTableWidgetItem(QString::number(rec.calculateFine(), 'f', 2)));
        }

        connect(returnButton, &QPushButton::clicked, [&]() {
            int row = table->currentRow();
            if (row < 0) {
                QMessageBox::warning(&dlg, "Return", "Please select a borrowed record.");
                return;
            }
            BorrowRecord rec = records[row];
            if (rec.isReturned()) {
                QMessageBox::information(&dlg, "Return", "This item has already been returned.");
                return;
            }
            QString message = manager->returnResource(currentUser->id, rec.resourceId);
            if (!message.isEmpty())
                QMessageBox::information(&dlg, "Return Processed", message);
            records = manager->borrowedRecords(currentUser->id);
            table->setRowCount(records.size());
            for (int r = 0; r < records.size(); ++r) {
                const BorrowRecord& rowRec = records[r];
                Resource* res = manager->resourceById(rowRec.resourceId);
                QString status = rowRec.isReturned() ? "Returned" : (stringToDate(rowRec.dueDate) < QDate::currentDate() ? "Overdue" : "Borrowed");
                table->setItem(r, 0, new QTableWidgetItem(QString::number(rowRec.recordId)));
                table->setItem(r, 1, new QTableWidgetItem(res ? res->title : "Unknown"));
                table->setItem(r, 2, new QTableWidgetItem(rowRec.issueDate));
                table->setItem(r, 3, new QTableWidgetItem(rowRec.dueDate));
                table->setItem(r, 4, new QTableWidgetItem(rowRec.returnDate.isEmpty() ? "—" : rowRec.returnDate));
                table->setItem(r, 5, new QTableWidgetItem(status));
                table->setItem(r, 6, new QTableWidgetItem(QString::number(rowRec.calculateFine(), 'f', 2)));
            }
            updateMemberSummary();
        });

        connect(closeButton, &QPushButton::clicked, &dlg, &QDialog::accept);
        dlg.exec();
    }

    void onNotifications() {
        QDialog dlg(this);
        dlg.setWindowTitle("Notifications");
        dlg.resize(720, 420);
        QVBoxLayout* mainLayout = new QVBoxLayout(&dlg);

        QTableWidget* table = new QTableWidget;
        table->setColumnCount(4);
        table->setHorizontalHeaderLabels({"ID", "Message", "Date", "Read"});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        mainLayout->addWidget(table);

        QPushButton* markReadButton = new QPushButton("Mark Read");
        QPushButton* closeButton = new QPushButton("Close");
        QHBoxLayout* actions = new QHBoxLayout;
        actions->addStretch();
        actions->addWidget(markReadButton);
        actions->addWidget(closeButton);
        mainLayout->addLayout(actions);

        QVector<Notification> notifications = manager->notificationsForUser(currentUser->id);
        table->setRowCount(notifications.size());
        for (int row = 0; row < notifications.size(); ++row) {
            const Notification& note = notifications[row];
            table->setItem(row, 0, new QTableWidgetItem(QString::number(note.id)));
            table->setItem(row, 1, new QTableWidgetItem(note.message));
            table->setItem(row, 2, new QTableWidgetItem(note.date));
            table->setItem(row, 3, new QTableWidgetItem(note.read ? "Yes" : "No"));
        }

        connect(markReadButton, &QPushButton::clicked, [&]() {
            int row = table->currentRow();
            if (row < 0) {
                QMessageBox::warning(&dlg, "Notifications", "Select a notification first.");
                return;
            }
            int id = table->item(row, 0)->text().toInt();
            manager->markNotificationRead(id);
            notifications = manager->notificationsForUser(currentUser->id);
            for (int r = 0; r < notifications.size(); ++r) {
                const Notification& note = notifications[r];
                table->setItem(r, 0, new QTableWidgetItem(QString::number(note.id)));
                table->setItem(r, 1, new QTableWidgetItem(note.message));
                table->setItem(r, 2, new QTableWidgetItem(note.date));
                table->setItem(r, 3, new QTableWidgetItem(note.read ? "Yes" : "No"));
            }
        });

        connect(closeButton, &QPushButton::clicked, &dlg, &QDialog::accept);
        dlg.exec();
    }

    void onRenewMembership() {
        QDialog dlg(this);
        dlg.setWindowTitle("Renew Membership");
        dlg.resize(420, 240);
        QVBoxLayout* layout = new QVBoxLayout(&dlg);
        QFormLayout* form = new QFormLayout;
        QComboBox* typeCombo = new QComboBox;
        typeCombo->addItems({"Silver", "Gold", "Platinum"});
        QComboBox* monthsCombo = new QComboBox;
        monthsCombo->addItems({"1", "3", "6", "12"});
        if (!currentUser->membershipType.isEmpty()) {
            int index = typeCombo->findText(currentUser->membershipType);
            if (index >= 0)
                typeCombo->setCurrentIndex(index);
        }
        form->addRow("Type:", typeCombo);
        form->addRow("Months:", monthsCombo);
        layout->addLayout(form);

        QHBoxLayout* buttons = new QHBoxLayout;
        QPushButton* renewButton = new QPushButton("Renew");
        QPushButton* cancelButton = new QPushButton("Cancel");
        buttons->addStretch();
        buttons->addWidget(renewButton);
        buttons->addWidget(cancelButton);
        layout->addLayout(buttons);

        connect(renewButton, &QPushButton::clicked, [&]() {
            QString type = typeCombo->currentText();
            int months = monthsCombo->currentText().toInt();
            manager->renewMembership(*currentUser, type, months);
            QMessageBox::information(&dlg, "Renew", "Membership renewed successfully.");
            updateMemberSummary();
            dlg.accept();
        });
        connect(cancelButton, &QPushButton::clicked, &dlg, &QDialog::reject);
        dlg.exec();
    }

    void onReserveOrWaitlistResource() {
        QDialog dlg(this);
        dlg.setWindowTitle("Reserve or Join Waitlist");
        dlg.resize(720, 420);
        QVBoxLayout* layout = new QVBoxLayout(&dlg);

        QLabel* header = new QLabel("Available Resources (Not Currently Borrowed)");
        header->setStyleSheet("font-weight: bold;");
        layout->addWidget(header);

        QTableWidget* table = new QTableWidget;
        table->setColumnCount(5);
        table->setHorizontalHeaderLabels({"ID", "Title", "Author", "Category", "Status"});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        layout->addWidget(table);

        auto populate = [&]() {
            table->setRowCount(manager->store.resources.size());
            for (int i = 0; i < manager->store.resources.size(); ++i) {
                const Resource& res = manager->store.resources[i];
                table->setItem(i, 0, new QTableWidgetItem(QString::number(res.id)));
                table->setItem(i, 1, new QTableWidgetItem(res.title));
                table->setItem(i, 2, new QTableWidgetItem(res.author));
                table->setItem(i, 3, new QTableWidgetItem(res.category));
                QString status;
                if (res.isAvailable()) {
                    status = "Available (Can borrow directly)";
                } else if (manager->isReserved(res.id)) {
                    int holder = manager->reservationHolder(res.id);
                    status = (holder == currentUser->id) ? "Reserved by you" : "Reserved by another";
                } else {
                    int pos = manager->waitlistPosition(currentUser->id, res.id);
                    if (pos > 0) {
                        status = QString("On waitlist (Position %1)").arg(pos);
                    } else {
                        status = "Unavailable (Can join waitlist)";
                    }
                }
                table->setItem(i, 4, new QTableWidgetItem(status));
            }
        };
        populate();

        QHBoxLayout* actions = new QHBoxLayout;
        QPushButton* reserveBtn = new QPushButton("Reserve");
        QPushButton* waitlistBtn = new QPushButton("Join Waitlist");
        QPushButton* closeBtn = new QPushButton("Close");
        actions->addStretch();
        actions->addWidget(reserveBtn);
        actions->addWidget(waitlistBtn);
        actions->addWidget(closeBtn);
        layout->addLayout(actions);

        connect(reserveBtn, &QPushButton::clicked, [&]() {
            int row = table->currentRow();
            if (row < 0) {
                QMessageBox::warning(&dlg, "Reserve", "Please select a resource.");
                return;
            }
            int resId = table->item(row, 0)->text().toInt();
            QString msg = manager->reserveResource(currentUser->id, resId);
            if (msg.isEmpty()) {
                QMessageBox::information(&dlg, "Reserve", "Reservation successful!");
                populate();
            } else {
                QMessageBox::warning(&dlg, "Reserve", msg);
            }
        });

        connect(waitlistBtn, &QPushButton::clicked, [&]() {
            int row = table->currentRow();
            if (row < 0) {
                QMessageBox::warning(&dlg, "Waitlist", "Please select a resource.");
                return;
            }
            int resId = table->item(row, 0)->text().toInt();
            QString msg = manager->addToWaitlist(currentUser->id, resId);
            if (msg.isEmpty()) {
                int pos = manager->waitlistPosition(currentUser->id, resId);
                QMessageBox::information(&dlg, "Waitlist", 
                    QString("Added to waitlist! Your position: %1").arg(pos));
                populate();
            } else {
                QMessageBox::warning(&dlg, "Waitlist", msg);
            }
        });

        connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
        dlg.exec();
    }

    void onRateResource() {
        QDialog dlg(this);
        dlg.setWindowTitle("Rate & Review Books");
        dlg.resize(720, 480);
        QVBoxLayout* layout = new QVBoxLayout(&dlg);

        QLabel* header = new QLabel("Rate and review books you have borrowed");
        header->setStyleSheet("font-weight: bold;");
        layout->addWidget(header);

        QTableWidget* table = new QTableWidget;
        table->setColumnCount(4);
        table->setHorizontalHeaderLabels({"Resource ID", "Title", "Rating Count", "Avg Rating"});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        layout->addWidget(table);

        auto populate = [&]() {
            table->setRowCount(manager->store.resources.size());
            for (int i = 0; i < manager->store.resources.size(); ++i) {
                const Resource& res = manager->store.resources[i];
                table->setItem(i, 0, new QTableWidgetItem(QString::number(res.id)));
                table->setItem(i, 1, new QTableWidgetItem(res.title));
                
                QVector<RatingEntry> ratings = manager->ratingsForResource(res.id);
                int count = ratings.size();
                double avgStars = 0.0;
                if (count > 0) {
                    int sum = 0;
                    for (const auto& r : ratings)
                        sum += r.stars;
                    avgStars = (double)sum / count;
                }
                table->setItem(i, 2, new QTableWidgetItem(QString::number(count)));
                table->setItem(i, 3, new QTableWidgetItem(avgStars > 0 ? 
                    QString::number(avgStars, 'f', 1) : "N/A"));
            }
        };
        populate();

        QHBoxLayout* actions = new QHBoxLayout;
        QPushButton* rateBtn = new QPushButton("Rate Selected");
        QPushButton* viewBtn = new QPushButton("View Reviews");
        QPushButton* closeBtn = new QPushButton("Close");
        actions->addStretch();
        actions->addWidget(rateBtn);
        actions->addWidget(viewBtn);
        actions->addWidget(closeBtn);
        layout->addLayout(actions);

        connect(rateBtn, &QPushButton::clicked, [&]() {
            int row = table->currentRow();
            if (row < 0) {
                QMessageBox::warning(&dlg, "Rate", "Please select a resource.");
                return;
            }
            int resId = table->item(row, 0)->text().toInt();
            QDialog ratingDlg(&dlg);
            ratingDlg.setWindowTitle("Submit Rating");
            ratingDlg.resize(400, 300);
            QVBoxLayout* rateLayout = new QVBoxLayout(&ratingDlg);
            
            QFormLayout* form = new QFormLayout;
            QSpinBox* starsBox = new QSpinBox;
            starsBox->setMinimum(1);
            starsBox->setMaximum(5);
            starsBox->setValue(5);
            QTextEdit* reviewEdit = new QTextEdit;
            form->addRow("Stars (1-5):", starsBox);
            form->addRow("Review (optional):", reviewEdit);
            rateLayout->addLayout(form);

            QHBoxLayout* btns = new QHBoxLayout;
            QPushButton* submitBtn = new QPushButton("Submit");
            QPushButton* cancelBtn = new QPushButton("Cancel");
            btns->addStretch();
            btns->addWidget(submitBtn);
            btns->addWidget(cancelBtn);
            rateLayout->addLayout(btns);

            connect(submitBtn, &QPushButton::clicked, [&]() {
                QString msg = manager->addRating(currentUser->id, resId, starsBox->value(), reviewEdit->toPlainText());
                if (msg.isEmpty()) {
                    QMessageBox::information(&ratingDlg, "Rating", "Thank you for your review!");
                    populate();
                    ratingDlg.accept();
                } else {
                    QMessageBox::warning(&ratingDlg, "Rating", msg);
                }
            });
            connect(cancelBtn, &QPushButton::clicked, &ratingDlg, &QDialog::reject);
            ratingDlg.exec();
        });

        connect(viewBtn, &QPushButton::clicked, [&]() {
            int row = table->currentRow();
            if (row < 0) {
                QMessageBox::warning(&dlg, "View Reviews", "Please select a resource.");
                return;
            }
            int resId = table->item(row, 0)->text().toInt();
            Resource* res = manager->resourceById(resId);
            QVector<RatingEntry> ratings = manager->ratingsForResource(resId);

            QString text = QString("Reviews for: %1 by %2\n\n").arg(res ? res->title : "Unknown", res ? res->author : "Unknown");
            if (ratings.isEmpty()) {
                text += "No reviews yet.";
            } else {
                for (const auto& r : ratings) {
                    text += QString("[%1★] %2 (by user %3)\n%4\n\n")
                        .arg(r.stars)
                        .arg(r.date)
                        .arg(r.userId)
                        .arg(r.review);
                }
            }
            QMessageBox::information(&dlg, "Reviews", text);
        });

        connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
        dlg.exec();
    }

    void onViewMyExtensions() {
        QDialog dlg(this);
        dlg.setWindowTitle("My Extension Requests");
        dlg.resize(720, 420);
        QVBoxLayout* layout = new QVBoxLayout(&dlg);

        QTableWidget* table = new QTableWidget;
        table->setColumnCount(7);
        table->setHorizontalHeaderLabels({"ID", "Resource", "Current Due", "Days Requested", "Status", "Request Date", "Response Date"});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        layout->addWidget(table);

        QVector<ExtensionRequest> extensions = manager->extensionsForUser(currentUser->id);
        table->setRowCount(extensions.size());
        for (int i = 0; i < extensions.size(); ++i) {
            const auto& ext = extensions[i];
            Resource* res = manager->resourceById(ext.resourceId);
            QString status;
            if (ext.status == ExtensionStatus::Pending)
                status = "Pending";
            else if (ext.status == ExtensionStatus::Approved)
                status = QString("Approved - New due: %1").arg(ext.newDueDate);
            else
                status = "Denied";
            
            table->setItem(i, 0, new QTableWidgetItem(QString::number(ext.requestId)));
            table->setItem(i, 1, new QTableWidgetItem(res ? res->title : "Unknown"));
            table->setItem(i, 2, new QTableWidgetItem(ext.currentDueDate));
            table->setItem(i, 3, new QTableWidgetItem(QString::number(ext.daysRequested)));
            table->setItem(i, 4, new QTableWidgetItem(status));
            table->setItem(i, 5, new QTableWidgetItem(ext.requestDate));
            table->setItem(i, 6, new QTableWidgetItem(ext.responseDate));
        }

        QPushButton* requestBtn = new QPushButton("Request Extension");
        QPushButton* closeBtn = new QPushButton("Close");
        QHBoxLayout* actions = new QHBoxLayout;
        actions->addStretch();
        actions->addWidget(requestBtn);
        actions->addWidget(closeBtn);
        layout->addLayout(actions);

        connect(requestBtn, &QPushButton::clicked, [&]() {
            QDialog reqDlg(&dlg);
            reqDlg.setWindowTitle("Request Due Date Extension");
            reqDlg.resize(420, 280);
            QVBoxLayout* reqLayout = new QVBoxLayout(&reqDlg);

            QFormLayout* form = new QFormLayout;
            QComboBox* resourceCombo = new QComboBox;
            
            // Build list of active borrowed records
            for (int j = 0; j < manager->store.records.size(); ++j) {
                const auto& rec = manager->store.records[j];
                if (rec.userId == currentUser->id && !rec.isReturned()) {
                    Resource* res = manager->resourceById(rec.resourceId);
                    resourceCombo->addItem(
                        res ? res->title : QString("Unknown (%1)").arg(rec.resourceId),
                        rec.resourceId);
                }
            }
            
            if (resourceCombo->count() == 0) {
                QMessageBox::information(&dlg, "Extension", "You have no active borrowed records to extend.");
                return;
            }
            
            QSpinBox* daysBox = new QSpinBox;
            daysBox->setMinimum(1);
            daysBox->setMaximum(14);
            daysBox->setValue(7);
            
            form->addRow("Resource:", resourceCombo);
            form->addRow("Days to Extend:", daysBox);
            reqLayout->addLayout(form);

            QHBoxLayout* btns = new QHBoxLayout;
            QPushButton* submitBtn = new QPushButton("Submit");
            QPushButton* cancelBtn = new QPushButton("Cancel");
            btns->addStretch();
            btns->addWidget(submitBtn);
            btns->addWidget(cancelBtn);
            reqLayout->addLayout(btns);

            connect(submitBtn, &QPushButton::clicked, [&]() {
                int resId = resourceCombo->currentData().toInt();
                QString msg = manager->requestExtension(currentUser->id, resId, daysBox->value());
                if (msg.isEmpty()) {
                    QMessageBox::information(&reqDlg, "Request", "Extension request submitted for admin review!");
                    reqDlg.accept();
                    extensions = manager->extensionsForUser(currentUser->id);
                    table->setRowCount(extensions.size());
                    for (int i = 0; i < extensions.size(); ++i) {
                        const auto& ext = extensions[i];
                        Resource* res = manager->resourceById(ext.resourceId);
                        QString status = ext.status == ExtensionStatus::Pending ? "Pending" : 
                                       ext.status == ExtensionStatus::Approved ? QString("Approved - %1").arg(ext.newDueDate) : "Denied";
                        table->setItem(i, 0, new QTableWidgetItem(QString::number(ext.requestId)));
                        table->setItem(i, 1, new QTableWidgetItem(res ? res->title : "Unknown"));
                        table->setItem(i, 2, new QTableWidgetItem(ext.currentDueDate));
                        table->setItem(i, 3, new QTableWidgetItem(QString::number(ext.daysRequested)));
                        table->setItem(i, 4, new QTableWidgetItem(status));
                        table->setItem(i, 5, new QTableWidgetItem(ext.requestDate));
                        table->setItem(i, 6, new QTableWidgetItem(ext.responseDate));
                    }
                } else {
                    QMessageBox::warning(&reqDlg, "Request", msg);
                }
            });
            connect(cancelBtn, &QPushButton::clicked, &reqDlg, &QDialog::reject);
            reqDlg.exec();
        });

        connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
        dlg.exec();
    }

    void onAdminViewPendingExtensions() {
        QDialog dlg(this);
        dlg.setWindowTitle("Pending Extension Requests");
        dlg.resize(820, 500);
        QVBoxLayout* layout = new QVBoxLayout(&dlg);

        QTableWidget* table = new QTableWidget;
        table->setColumnCount(8);
        table->setHorizontalHeaderLabels({"ID", "User", "Resource", "Current Due", "Days Requested", "Request Date", "Status", "Action"});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        layout->addWidget(table);

        auto populate = [&]() {
            QVector<ExtensionRequest> pending = manager->pendingExtensions();
            table->setRowCount(pending.size());
            for (int i = 0; i < pending.size(); ++i) {
                const auto& ext = pending[i];
                User* user = manager->userById(ext.userId);
                Resource* res = manager->resourceById(ext.resourceId);
                table->setItem(i, 0, new QTableWidgetItem(QString::number(ext.requestId)));
                table->setItem(i, 1, new QTableWidgetItem(user ? (user->firstName + " " + user->lastName) : "Unknown"));
                table->setItem(i, 2, new QTableWidgetItem(res ? res->title : "Unknown"));
                table->setItem(i, 3, new QTableWidgetItem(ext.currentDueDate));
                table->setItem(i, 4, new QTableWidgetItem(QString::number(ext.daysRequested)));
                table->setItem(i, 5, new QTableWidgetItem(ext.requestDate));
                table->setItem(i, 6, new QTableWidgetItem("Pending"));
                
                QPushButton* actionBtn = new QPushButton("Approve/Deny");
                table->setCellWidget(i, 7, actionBtn);
                
                // Capture by value to avoid stale references
                int extRequestId = ext.requestId;
                QString resourceTitle = res ? res->title : "Unknown";
                int daysReq = ext.daysRequested;
                QString currentDue = ext.currentDueDate;
                
                connect(actionBtn, &QPushButton::clicked, [this, extRequestId, resourceTitle, daysReq, currentDue, &dlg]() {
                    QDialog actionDlg(&dlg);
                    actionDlg.setWindowTitle("Review Extension Request");
                    actionDlg.resize(400, 250);
                    QVBoxLayout* actLayout = new QVBoxLayout(&actionDlg);
                    
                    QLabel* infoLabel = new QLabel(QString("Resource: %1\nDays Requested: %2 days\nCurrent Due: %3")
                        .arg(resourceTitle)
                        .arg(daysReq)
                        .arg(currentDue));
                    actLayout->addWidget(infoLabel);

                    QTextEdit* notesEdit = new QTextEdit;
                    notesEdit->setPlaceholderText("Admin notes (optional)");
                    actLayout->addWidget(notesEdit);

                    QHBoxLayout* btns = new QHBoxLayout;
                    QPushButton* approveBtn = new QPushButton("Approve");
                    QPushButton* denyBtn = new QPushButton("Deny");
                    btns->addStretch();
                    btns->addWidget(approveBtn);
                    btns->addWidget(denyBtn);
                    actLayout->addLayout(btns);

                    connect(approveBtn, &QPushButton::clicked, [this, extRequestId, &actionDlg]() {
                        manager->processExtension(extRequestId, true, actionDlg.findChild<QTextEdit*>()->toPlainText());
                        QMessageBox::information(&actionDlg, "Success", "Extension approved!");
                        actionDlg.accept();
                    });
                    connect(denyBtn, &QPushButton::clicked, [this, extRequestId, &actionDlg]() {
                        manager->processExtension(extRequestId, false, actionDlg.findChild<QTextEdit*>()->toPlainText());
                        QMessageBox::information(&actionDlg, "Success", "Extension denied!");
                        actionDlg.accept();
                    });
                    actionDlg.exec();
                });
            }
        };
        populate();

        QPushButton* closeBtn = new QPushButton("Close");
        layout->addWidget(closeBtn, 0, Qt::AlignRight);
        connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
        dlg.exec();
    }

    void onAdminManageWaitlists() {
        QDialog dlg(this);
        dlg.setWindowTitle("Reservations & Waitlists");
        dlg.resize(820, 480);
        QVBoxLayout* layout = new QVBoxLayout(&dlg);

        QTabWidget* tabs = new QTabWidget;

        // Reservations tab
        QWidget* reservTab = new QWidget;
        QVBoxLayout* reservLayout = new QVBoxLayout(reservTab);
        QTableWidget* reservTable = new QTableWidget;
        reservTable->setColumnCount(4);
        reservTable->setHorizontalHeaderLabels({"Resource ID", "Resource Title", "Reserved By", "Member Name"});
        reservTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        reservLayout->addWidget(reservTable);

        auto populateReservations = [&]() {
            reservTable->setRowCount(manager->store.reservations.size());
            for (int i = 0; i < manager->store.reservations.size(); ++i) {
                const auto& res = manager->store.reservations[i];
                Resource* resource = manager->resourceById(res.resourceId);
                User* user = manager->userById(res.userId);
                reservTable->setItem(i, 0, new QTableWidgetItem(QString::number(res.resourceId)));
                reservTable->setItem(i, 1, new QTableWidgetItem(resource ? resource->title : "Unknown"));
                reservTable->setItem(i, 2, new QTableWidgetItem(QString::number(res.userId)));
                reservTable->setItem(i, 3, new QTableWidgetItem(user ? (user->firstName + " " + user->lastName) : "Unknown"));
            }
        };
        populateReservations();
        tabs->addTab(reservTab, "Reservations");

        // Waitlists tab
        QWidget* waitTab = new QWidget;
        QVBoxLayout* waitLayout = new QVBoxLayout(waitTab);
        QTableWidget* waitTable = new QTableWidget;
        waitTable->setColumnCount(4);
        waitTable->setHorizontalHeaderLabels({"Resource ID", "Resource Title", "Position", "User ID"});
        waitTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        waitLayout->addWidget(waitTable);

        auto populateWaitlist = [&]() {
            waitTable->setRowCount(manager->store.waitlist.size());
            int pos = 1;
            int lastResId = -1;
            for (int i = 0; i < manager->store.waitlist.size(); ++i) {
                const auto& item = manager->store.waitlist[i];
                if (item.resourceId != lastResId) {
                    pos = 1;
                    lastResId = item.resourceId;
                } else {
                    pos++;
                }
                Resource* resource = manager->resourceById(item.resourceId);
                waitTable->setItem(i, 0, new QTableWidgetItem(QString::number(item.resourceId)));
                waitTable->setItem(i, 1, new QTableWidgetItem(resource ? resource->title : "Unknown"));
                waitTable->setItem(i, 2, new QTableWidgetItem(QString::number(pos)));
                waitTable->setItem(i, 3, new QTableWidgetItem(QString::number(item.userId)));
            }
        };
        populateWaitlist();
        tabs->addTab(waitTab, "Waitlists");

        layout->addWidget(tabs);

        QPushButton* closeBtn = new QPushButton("Close");
        layout->addWidget(closeBtn, 0, Qt::AlignRight);
        connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
        dlg.exec();
    }

    void onPayFine() {
        bool ok = false;
        double amount = QInputDialog::getDouble(this, "Pay Fine", "Amount to pay:", 0.0, 0.0, 100000.0, 2, &ok);
        if (!ok)
            return;
        QString error = manager->payFine(*currentUser, amount);
        if (!error.isEmpty()) {
            QMessageBox::warning(this, "Pay Fine", error);
        } else {
            QMessageBox::information(this, "Pay Fine", "Payment received.");
            updateMemberSummary();
        }
    }

    void onAdminAddResource() {
        QDialog dlg(this);
        dlg.setWindowTitle("Add Resource");
        dlg.resize(520, 520);
        QVBoxLayout* main = new QVBoxLayout(&dlg);
        QFormLayout* form = new QFormLayout;
        QLineEdit* titleEdit = new QLineEdit;
        QLineEdit* authorEdit = new QLineEdit;
        QLineEdit* categoryEdit = new QLineEdit;
        QComboBox* typeCombo = new QComboBox;
        typeCombo->addItems({"Book", "EBook"});
        QLineEdit* isbnEdit = new QLineEdit;
        QLineEdit* publisherEdit = new QLineEdit;
        QLineEdit* pagesEdit = new QLineEdit;
        QLineEdit* locationEdit = new QLineEdit;
        QLineEdit* fileSizeEdit = new QLineEdit;
        QLineEdit* formatEdit = new QLineEdit;
        QLineEdit* linkEdit = new QLineEdit;
        QLineEdit* licenseEdit = new QLineEdit;

        form->addRow("Type:", typeCombo);
        form->addRow("Title:", titleEdit);
        form->addRow("Author:", authorEdit);
        form->addRow("Category:", categoryEdit);
        form->addRow("ISBN:", isbnEdit);
        form->addRow("Publisher:", publisherEdit);
        form->addRow("Pages:", pagesEdit);
        form->addRow("Location:", locationEdit);
        form->addRow("File size (MB):", fileSizeEdit);
        form->addRow("Format:", formatEdit);
        form->addRow("Download link:", linkEdit);
        form->addRow("License count:", licenseEdit);
        main->addLayout(form);

        QPushButton* saveButton = new QPushButton("Save");
        QPushButton* cancelButton = new QPushButton("Cancel");
        QHBoxLayout* actions = new QHBoxLayout;
        actions->addStretch();
        actions->addWidget(saveButton);
        actions->addWidget(cancelButton);
        main->addLayout(actions);

        auto updateFieldVisibility = [&]() {
            bool isBook = typeCombo->currentText() == "Book";
            isbnEdit->setEnabled(isBook);
            publisherEdit->setEnabled(isBook);
            pagesEdit->setEnabled(isBook);
            locationEdit->setEnabled(isBook);
            fileSizeEdit->setEnabled(!isBook);
            formatEdit->setEnabled(!isBook);
            linkEdit->setEnabled(!isBook);
            licenseEdit->setEnabled(!isBook);
        };
        updateFieldVisibility();
        connect(typeCombo, &QComboBox::currentTextChanged, updateFieldVisibility);

        connect(saveButton, &QPushButton::clicked, [&]() {
            Resource resource;
            resource.type = typeCombo->currentText();
            resource.title = titleEdit->text().trimmed();
            resource.author = authorEdit->text().trimmed();
            resource.category = categoryEdit->text().trimmed();
            resource.digital = resource.type == "EBook";
            resource.isbn = isbnEdit->text().trimmed();
            resource.publisher = publisherEdit->text().trimmed();
            resource.pages = pagesEdit->text().toInt();
            resource.location = locationEdit->text().trimmed();
            resource.fileSize = fileSizeEdit->text().toFloat();
            resource.format = formatEdit->text().trimmed();
            resource.downloadLink = linkEdit->text().trimmed();
            resource.licenseCount = licenseEdit->text().toInt();
            if (resource.digital && resource.licenseCount <= 0)
                resource.licenseCount = 1;
            if (resource.title.isEmpty() || resource.author.isEmpty() || resource.category.isEmpty()) {
                QMessageBox::warning(&dlg, "Add Resource", "Title, author, and category are required.");
                return;
            }
            QString error = manager->addResource(resource);
            if (!error.isEmpty()) {
                QMessageBox::warning(&dlg, "Add Resource", error);
                return;
            }
            QMessageBox::information(&dlg, "Add Resource", "Resource added successfully.");
            dlg.accept();
        });
        connect(cancelButton, &QPushButton::clicked, &dlg, &QDialog::reject);
        dlg.exec();
    }

    void onAdminViewResources() {
        QDialog dlg(this);
        dlg.setWindowTitle("All Resources");
        dlg.resize(900, 520);
        QVBoxLayout* layout = new QVBoxLayout(&dlg);
        QTableWidget* table = new QTableWidget;
        table->setColumnCount(8);
        table->setHorizontalHeaderLabels({"ID", "Title", "Author", "Category", "Type", "Availability", "Location", "Extra"});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        layout->addWidget(table);
        QPushButton* closeButton = new QPushButton("Close");
        layout->addWidget(closeButton, 0, Qt::AlignRight);

        table->setRowCount(manager->store.resources.size());
        for (int row = 0; row < manager->store.resources.size(); ++row) {
            const Resource& res = manager->store.resources[row];
            table->setItem(row, 0, new QTableWidgetItem(QString::number(res.id)));
            table->setItem(row, 1, new QTableWidgetItem(res.title));
            table->setItem(row, 2, new QTableWidgetItem(res.author));
            table->setItem(row, 3, new QTableWidgetItem(res.category));
            table->setItem(row, 4, new QTableWidgetItem(res.displayType()));
            table->setItem(row, 5, new QTableWidgetItem(res.availabilityText()));
            table->setItem(row, 6, new QTableWidgetItem(res.location));
            QString extra = res.digital ? QString("Format: %1, Licenses: %2").arg(res.format).arg(res.licenseCount)
                                       : QString("ISBN: %1, Publisher: %2, Pages: %3").arg(res.isbn).arg(res.publisher).arg(res.pages);
            table->setItem(row, 7, new QTableWidgetItem(extra));
        }
        connect(closeButton, &QPushButton::clicked, &dlg, &QDialog::accept);
        dlg.exec();
    }

    void onAdminViewMembers() {
        QDialog dlg(this);
        dlg.setWindowTitle("All Members");
        dlg.resize(860, 500);
        QVBoxLayout* layout = new QVBoxLayout(&dlg);
        QTableWidget* table = new QTableWidget;
        table->setColumnCount(7);
        table->setHorizontalHeaderLabels({"ID", "Name", "Username", "Email", "Phone", "Membership", "Balance"});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        layout->addWidget(table);
        QPushButton* closeButton = new QPushButton("Close");
        layout->addWidget(closeButton, 0, Qt::AlignRight);

        QVector<User> members = manager->allMembers();
        table->setRowCount(members.size());
        for (int row = 0; row < members.size(); ++row) {
            const User& u = members[row];
            table->setItem(row, 0, new QTableWidgetItem(QString::number(u.id)));
            table->setItem(row, 1, new QTableWidgetItem(u.firstName + " " + u.lastName));
            table->setItem(row, 2, new QTableWidgetItem(u.username));
            table->setItem(row, 3, new QTableWidgetItem(u.email));
            table->setItem(row, 4, new QTableWidgetItem(u.phone));
            table->setItem(row, 5, new QTableWidgetItem(u.membershipType));
            table->setItem(row, 6, new QTableWidgetItem(QString::number(u.balance, 'f', 2)));
        }
        connect(closeButton, &QPushButton::clicked, &dlg, &QDialog::accept);
        dlg.exec();
    }

private:
    QStackedWidget* stacked;
    QWidget* initPage;
    QWidget* loginPage;
    QWidget* registerPage;
    QWidget* memberPage;
    QWidget* adminPage;
    QLineEdit* loginUsername;
    QLineEdit* loginPassword;
    QLineEdit* regUsername;
    QLineEdit* regPassword;
    QLineEdit* regFirstName;
    QLineEdit* regLastName;
    QLineEdit* regEmail;
    QLineEdit* regPhone;
    QLineEdit* regAddress;
    QLabel* memberSummary;
    QLabel* adminSummary;
    LibraryManager* manager;
    User* currentUser = nullptr;

    void createLoginPage() {
        loginPage = new QWidget;
        QVBoxLayout* main = new QVBoxLayout(loginPage);
        main->setSpacing(16);
        main->setContentsMargins(40, 60, 40, 60);

        QLabel* appTitle = new QLabel("Library Management System");
        appTitle->setAlignment(Qt::AlignCenter);
        appTitle->setProperty("level", "title");
        main->addWidget(appTitle);

        QLabel* subtitle = new QLabel("Secure Library Resource Management Platform");
        subtitle->setAlignment(Qt::AlignCenter);
        subtitle->setStyleSheet("color: #64748b; font-size: 13px; margin-bottom: 32px;");
        main->addWidget(subtitle);

        QGroupBox* loginBox = new QGroupBox("User Login");
        loginBox->setStyleSheet("QGroupBox { padding-top: 16px; }");
        QFormLayout* form = new QFormLayout(loginBox);
        form->setSpacing(12);
        form->setContentsMargins(20, 20, 20, 20);
        
        QLabel* userLabel = new QLabel("Username:");
        userLabel->setProperty("level", "subtitle");
        loginUsername = new QLineEdit;
        loginUsername->setPlaceholderText("Enter your username");
        loginUsername->setMinimumHeight(40);
        
        QLabel* passLabel = new QLabel("Password:");
        passLabel->setProperty("level", "subtitle");
        loginPassword = new QLineEdit;
        loginPassword->setPlaceholderText("Enter your password");
        loginPassword->setEchoMode(QLineEdit::Password);
        loginPassword->setMinimumHeight(40);
        
        form->addRow(userLabel, loginUsername);
        form->addRow(passLabel, loginPassword);
        main->addWidget(loginBox);

        main->addSpacing(24);

        QPushButton* loginButton = new QPushButton("Login");
        loginButton->setMinimumHeight(44);
        loginButton->setFont(QFont("Arial", 12, QFont::Bold));
        
        QPushButton* memberRegButton = new QPushButton("Register as Member");
        memberRegButton->setMinimumHeight(40);
        memberRegButton->setStyleSheet(R"(
            QPushButton {
                background-color: #059669;
            }
            QPushButton:hover {
                background-color: #047857;
            }
        )");
        
        QPushButton* adminRegButton = new QPushButton("Register as Admin");
        adminRegButton->setMinimumHeight(40);
        adminRegButton->setStyleSheet(R"(
            QPushButton {
                background-color: #7c3aed;
            }
            QPushButton:hover {
                background-color: #6d28d9;
            }
        )");

        QVBoxLayout* buttonLayout = new QVBoxLayout;
        buttonLayout->setSpacing(12);
        buttonLayout->addWidget(loginButton);
        buttonLayout->addWidget(memberRegButton);
        buttonLayout->addWidget(adminRegButton);
        main->addLayout(buttonLayout);

        main->addStretch();

        connect(loginButton, &QPushButton::clicked, this, &MainWindow::onLogin);
        connect(memberRegButton, &QPushButton::clicked, this, &MainWindow::onShowRegister);
        connect(adminRegButton, &QPushButton::clicked, [this]() { onShowAdminRegister(false); });
    }

    void createInitPage() {
        initPage = new QWidget;
        QVBoxLayout* main = new QVBoxLayout(initPage);
        main->setSpacing(24);
        main->setContentsMargins(60, 80, 60, 80);

        QLabel* title = new QLabel("System Initialization");
        title->setProperty("level", "title");
        title->setAlignment(Qt::AlignCenter);
        main->addWidget(title);

        QGroupBox* infoBox = new QGroupBox("Welcome to Library Management System");
        QVBoxLayout* infoLayout = new QVBoxLayout(infoBox);
        infoLayout->setContentsMargins(30, 20, 30, 20);
        infoLayout->setSpacing(12);
        
        QLabel* message1 = new QLabel("This is your first time running the application.");
        message1->setWordWrap(true);
        message1->setStyleSheet("color: #1e293b; font-size: 13px;");
        
        QLabel* message2 = new QLabel("No administrator account exists yet. Please register as a Super Admin to initialize the system and unlock all administrative features.");
        message2->setWordWrap(true);
        message2->setStyleSheet("color: #475569; font-size: 12px;");
        
        infoLayout->addWidget(message1);
        infoLayout->addWidget(message2);
        main->addWidget(infoBox);

        main->addSpacing(16);

        QPushButton* registerAdminButton = new QPushButton("🔐 Register as Super Admin");
        registerAdminButton->setMinimumHeight(48);
        registerAdminButton->setFont(QFont("Arial", 12, QFont::Bold));
        registerAdminButton->setStyleSheet(R"(
            QPushButton {
                background-color: #dc2626;
            }
            QPushButton:hover {
                background-color: #b91c1c;
            }
        )");

        QPushButton* skipButton = new QPushButton("Skip to Login");
        skipButton->setMinimumHeight(42);
        skipButton->setStyleSheet(R"(
            QPushButton {
                background-color: #64748b;
            }
            QPushButton:hover {
                background-color: #475569;
            }
        )");

        QVBoxLayout* buttonLayout = new QVBoxLayout;
        buttonLayout->setSpacing(12);
        buttonLayout->addWidget(registerAdminButton);
        buttonLayout->addWidget(skipButton);
        main->addLayout(buttonLayout);

        main->addStretch();

        connect(registerAdminButton, &QPushButton::clicked, [this]() { onShowAdminRegister(true); });
        connect(skipButton, &QPushButton::clicked, [this]() { stacked->setCurrentWidget(loginPage); });
    }

    void createRegisterPage() {
        registerPage = new QWidget;
        QVBoxLayout* main = new QVBoxLayout(registerPage);
        main->setSpacing(16);
        main->setContentsMargins(40, 40, 40, 40);

        QLabel* header = new QLabel("Member Registration");
        header->setProperty("level", "title");
        header->setAlignment(Qt::AlignCenter);
        main->addWidget(header);

        QLabel* subtitle = new QLabel("Create your account to start managing library resources");
        subtitle->setAlignment(Qt::AlignCenter);
        subtitle->setStyleSheet("color: #64748b; font-size: 12px; margin-bottom: 24px;");
        main->addWidget(subtitle);

        QGroupBox* formBox = new QGroupBox("Account Information");
        formBox->setStyleSheet("QGroupBox { padding-top: 16px; }");
        QFormLayout* form = new QFormLayout(formBox);
        form->setSpacing(14);
        form->setContentsMargins(20, 20, 20, 20);

        regUsername = new QLineEdit;
        regUsername->setPlaceholderText("Choose a unique username");
        regUsername->setMinimumHeight(38);
        
        regPassword = new QLineEdit;
        regPassword->setPlaceholderText("Enter a strong password");
        regPassword->setEchoMode(QLineEdit::Password);
        regPassword->setMinimumHeight(38);
        
        regFirstName = new QLineEdit;
        regFirstName->setPlaceholderText("Your first name");
        regFirstName->setMinimumHeight(38);
        
        regLastName = new QLineEdit;
        regLastName->setPlaceholderText("Your last name");
        regLastName->setMinimumHeight(38);
        
        regEmail = new QLineEdit;
        regEmail->setPlaceholderText("your@email.com");
        regEmail->setMinimumHeight(38);
        
        regPhone = new QLineEdit;
        regPhone->setPlaceholderText("03XX-XXXXXXX");
        regPhone->setMinimumHeight(38);
        
        regAddress = new QLineEdit;
        regAddress->setPlaceholderText("Your address");
        regAddress->setMinimumHeight(38);

        form->addRow("Username:", regUsername);
        form->addRow("Password:", regPassword);
        form->addRow("First Name:", regFirstName);
        form->addRow("Last Name:", regLastName);
        form->addRow("Email:", regEmail);
        form->addRow("Phone:", regPhone);
        form->addRow("Address:", regAddress);
        
        main->addWidget(formBox);
        main->addSpacing(24);

        QPushButton* submit = new QPushButton("Register Account");
        submit->setMinimumHeight(44);
        submit->setFont(QFont("Arial", 12, QFont::Bold));
        
        QPushButton* back = new QPushButton("Back to Login");
        back->setMinimumHeight(40);
        back->setStyleSheet(R"(
            QPushButton {
                background-color: #64748b;
            }
            QPushButton:hover {
                background-color: #475569;
            }
        )");

        QVBoxLayout* buttonLayout = new QVBoxLayout;
        buttonLayout->setSpacing(12);
        buttonLayout->addWidget(submit);
        buttonLayout->addWidget(back);
        main->addLayout(buttonLayout);

        main->addStretch();

        connect(submit, &QPushButton::clicked, this, &MainWindow::onRegisterSubmit);
        connect(back, &QPushButton::clicked, [this]() { stacked->setCurrentWidget(loginPage); });
    }

    void createMemberPage() {
        memberPage = new QWidget;
        QVBoxLayout* main = new QVBoxLayout(memberPage);
        main->setSpacing(16);
        main->setContentsMargins(30, 30, 30, 30);

        QLabel* header = new QLabel("Member Dashboard");
        header->setProperty("level", "title");
        header->setAlignment(Qt::AlignCenter);
        main->addWidget(header);

        QGroupBox* summaryBox = new QGroupBox("Your Profile Summary");
        QVBoxLayout* summaryLayout = new QVBoxLayout(summaryBox);
        summaryLayout->setContentsMargins(20, 20, 20, 20);
        memberSummary = new QLabel;
        memberSummary->setWordWrap(true);
        memberSummary->setStyleSheet("QLabel { color: #1e293b; line-height: 1.8; }");
        summaryLayout->addWidget(memberSummary);
        main->addWidget(summaryBox);

        QLabel* actionsLabel = new QLabel("Quick Actions");
        actionsLabel->setProperty("level", "subtitle");
        main->addWidget(actionsLabel);

        QPushButton* browse = new QPushButton("Browse & Borrow Resources");
        QPushButton* borrowed = new QPushButton("My Borrowed Books");
        QPushButton* reserve = new QPushButton("Reserve / Join Waitlist");
        QPushButton* ratings = new QPushButton("Rate & Review Books");
        QPushButton* extensions = new QPushButton("Extension Requests");
        QPushButton* notifications = new QPushButton("Notifications");
        QPushButton* renew = new QPushButton("Renew Membership");
        QPushButton* payFine = new QPushButton("Pay Fines / Add Funds");
        QPushButton* logout = new QPushButton("Logout");

        for (auto btn : QVector<QPushButton*>{browse, borrowed, reserve, ratings, extensions, notifications, renew, payFine}) {
            btn->setMinimumHeight(42);
            btn->setStyleSheet(R"(
                QPushButton {
                    background-color: #2563eb;
                    text-align: left;
                    padding-left: 16px;
                }
                QPushButton:hover {
                    background-color: #1d4ed8;
                }
            )");
        }

        logout->setMinimumHeight(42);
        logout->setStyleSheet(R"(
            QPushButton {
                background-color: #dc2626;
            }
            QPushButton:hover {
                background-color: #b91c1c;
            }
        )");

        QGridLayout* gridLayout = new QGridLayout;
        gridLayout->setSpacing(12);
        gridLayout->addWidget(browse, 0, 0);
        gridLayout->addWidget(borrowed, 0, 1);
        gridLayout->addWidget(reserve, 1, 0);
        gridLayout->addWidget(ratings, 1, 1);
        gridLayout->addWidget(extensions, 2, 0);
        gridLayout->addWidget(notifications, 2, 1);
        gridLayout->addWidget(renew, 3, 0);
        gridLayout->addWidget(payFine, 3, 1);
        main->addLayout(gridLayout);

        main->addSpacing(12);
        main->addWidget(logout);
        main->addStretch();

        connect(browse, &QPushButton::clicked, this, &MainWindow::onBrowseResources);
        connect(borrowed, &QPushButton::clicked, this, &MainWindow::onViewBorrowed);
        connect(reserve, &QPushButton::clicked, this, &MainWindow::onReserveOrWaitlistResource);
        connect(ratings, &QPushButton::clicked, this, &MainWindow::onRateResource);
        connect(extensions, &QPushButton::clicked, this, &MainWindow::onViewMyExtensions);
        connect(notifications, &QPushButton::clicked, this, &MainWindow::onNotifications);
        connect(renew, &QPushButton::clicked, this, &MainWindow::onRenewMembership);
        connect(payFine, &QPushButton::clicked, this, &MainWindow::onPayFine);
        connect(logout, &QPushButton::clicked, this, &MainWindow::onLogout);
    }

    void createAdminPage() {
        adminPage = new QWidget;
        QVBoxLayout* main = new QVBoxLayout(adminPage);
        main->setSpacing(16);
        main->setContentsMargins(30, 30, 30, 30);

        QLabel* header = new QLabel("Administrator Dashboard");
        header->setProperty("level", "title");
        header->setAlignment(Qt::AlignCenter);
        main->addWidget(header);

        QGroupBox* statsBox = new QGroupBox("System Statistics");
        QVBoxLayout* statsLayout = new QVBoxLayout(statsBox);
        statsLayout->setContentsMargins(20, 20, 20, 20);
        adminSummary = new QLabel;
        adminSummary->setWordWrap(true);
        adminSummary->setStyleSheet("QLabel { color: #1e293b; line-height: 1.8; }");
        statsLayout->addWidget(adminSummary);
        main->addWidget(statsBox);

        QLabel* actionsLabel = new QLabel("Administrative Functions");
        actionsLabel->setProperty("level", "subtitle");
        main->addWidget(actionsLabel);

        QPushButton* addResource = new QPushButton("Add New Resource");
        QPushButton* viewResources = new QPushButton("View All Resources");
        QPushButton* viewMembers = new QPushButton("View All Members");
        QPushButton* pendingExtensions = new QPushButton("Pending Extension Requests");
        QPushButton* manageWaitlists = new QPushButton("Reservations & Waitlists");
        QPushButton* logout = new QPushButton("Logout");

        for (auto btn : QVector<QPushButton*>{addResource, viewResources, viewMembers, pendingExtensions, manageWaitlists}) {
            btn->setMinimumHeight(42);
            btn->setStyleSheet(R"(
                QPushButton {
                    background-color: #7c3aed;
                    text-align: left;
                    padding-left: 16px;
                }
                QPushButton:hover {
                    background-color: #6d28d9;
                }
            )");
        }

        logout->setMinimumHeight(42);
        logout->setStyleSheet(R"(
            QPushButton {
                background-color: #dc2626;
            }
            QPushButton:hover {
                background-color: #b91c1c;
            }
        )");

        QGridLayout* gridLayout = new QGridLayout;
        gridLayout->setSpacing(12);
        gridLayout->addWidget(addResource, 0, 0);
        gridLayout->addWidget(viewResources, 0, 1);
        gridLayout->addWidget(viewMembers, 1, 0);
        gridLayout->addWidget(pendingExtensions, 1, 1);
        gridLayout->addWidget(manageWaitlists, 2, 0, 1, 2);
        main->addLayout(gridLayout);

        main->addSpacing(12);
        main->addWidget(logout);
        main->addStretch();

        connect(addResource, &QPushButton::clicked, this, &MainWindow::onAdminAddResource);
        connect(viewResources, &QPushButton::clicked, this, &MainWindow::onAdminViewResources);
        connect(viewMembers, &QPushButton::clicked, this, &MainWindow::onAdminViewMembers);
        connect(pendingExtensions, &QPushButton::clicked, this, &MainWindow::onAdminViewPendingExtensions);
        connect(manageWaitlists, &QPushButton::clicked, this, &MainWindow::onAdminManageWaitlists);
        connect(logout, &QPushButton::clicked, this, &MainWindow::onLogout);
    }

    void updateMemberSummary() {
        if (!currentUser)
            return;
        int borrowed = manager->activeBorrowCount(currentUser->id);
        QString fineText = manager->outstandingFine(*currentUser);
        QString summary = QString("Welcome %1 %2\nMembership: %3 (Expires %4)\nBorrowed: %5 / %6\nOutstanding fine: %7")
            .arg(currentUser->firstName)
            .arg(currentUser->lastName)
            .arg(currentUser->membershipType)
            .arg(dateToString(currentUser->membershipExpiry))
            .arg(borrowed)
            .arg(currentUser->borrowLimit())
            .arg(fineText);
        memberSummary->setText(summary);
    }

    void updateAdminSummary() {
        if (!currentUser)
            return;
        QString summary = QString("Welcome Admin %1 %2\nTotal resources: %3\nTotal members: %4")
            .arg(currentUser->firstName)
            .arg(currentUser->lastName)
            .arg(manager->store.resources.size())
            .arg(manager->allMembers().size());
        adminSummary->setText(summary);
    }
};

#include "LibraryManagementQtApp.moc"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}
