#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <cmath>
#include "Usermanager.h"
#include "LibraryManager.h"
#include "BorrowRecord.h"
#include "ReportGenerator.h"
#include "Validator.h"
#include "SYSTEMEXCEPTIONS.h"
#include "InputHelper.h"
#include "AuditLogger.h"
#include "SecurityUtils.h"
#include "Notification.h"
#include "Book.h"
#include "Ebook.h"
#include "BookRating.h"
#include "BookCopies.h"
#include "BorrowExtension.h"
#include "AdvancedSearch.h"
#include "AdminAnalytics.h"

using namespace std;

namespace {
int getNextResourceID(const LibraryManager& lib)
{
    int maxID = 0;
    for (const auto* resource : lib.getResources()) {
        if (resource && resource->getResourceID() > maxID)
            maxID = resource->getResourceID();
    }
    return maxID + 1;
}

float readValidatedFileSize(const string& prompt)
{
    while (true) {
        try {
            const string value = InputHelper::readLine(prompt);
            size_t processed = 0;
            const float size = stof(value, &processed);
            if (processed != value.length())
                throw ValidationException("Please enter a valid file size.");
            Validator::validateFileSize(size);
            return size;
        }
        catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
            if (!InputHelper::askRetryOrExit())
                throw OperationCancelledException();
        }
    }
}

double readValidatedAmount(const string& prompt)
{
    while (true) {
        try {
            const string value = InputHelper::readLine(prompt);
            size_t processed = 0;
            const double amount = stod(value, &processed);
            if (processed != value.length() || amount <= 0)
                throw ValidationException("Amount must be greater than zero.");
            return amount;
        }
        catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
            if (!InputHelper::askRetryOrExit())
                throw OperationCancelledException();
        }
    }
}

void addResourceFromInput(LibraryManager& lib)
{
    cout << "\n==================================\n";
    cout << "       ADD NEW RESOURCE           \n";
    cout << "==================================\n";
    cout << "1. Printed Book\n";
    cout << "2. EBook\n";
    cout << "0. Back\n";

    const int type = InputHelper::readMenuChoice("Choice: ", 0, 2);
    if (type == 0)
        return;

    const int id = getNextResourceID(lib);
    const string title = InputHelper::readValidatedLine("Title: ", Validator::validateString);
    const string author = InputHelper::readValidatedLine("Author: ", Validator::validateName);
    const string category = InputHelper::readValidatedLine("Category: ", Validator::validateCategory);

    if (type == 1) {
        const string isbn = InputHelper::readValidatedLine("ISBN (10 or 13 digits): ", Validator::validateISBN);
        const string publisher = InputHelper::readValidatedLine("Publisher: ", Validator::validateString);
        const int pages = InputHelper::readValidatedInt("Pages: ", Validator::validatePages);
        const string location = InputHelper::readValidatedLine("Physical location / branch: ", Validator::validateString);
        lib.addResource(new Book(id, title, author, category, true, isbn, publisher, pages, location));
    }
    else {
        const float fileSize = readValidatedFileSize("File size in MB: ");
        const string format = InputHelper::readValidatedLine("Format (PDF/EPUB): ", Validator::validateFileFormat);
        const int licenseCount = [&]() {
            while (true) {
                int count = InputHelper::readValidatedInt("Concurrent license count: ", Validator::validatePositiveOrZero);
                if (count > 0)
                    return count;
                cout << "License count must be at least 1." << endl;
            }
        }();
        const string link = InputHelper::readValidatedLine("Download link: ", Validator::validateUrl);
        lib.addResource(new EBook(id, title, author, category, true, fileSize, format, link, licenseCount, "Online"));
    }

    cout << "New Resource ID: " << id << "\n";
}

void rateBook(LibraryManager& lib, Member* member)
{
    if (!member)
        throw AuthorizationException("Only members can rate books.");

    cout << "\n==================================\n";
    cout << "    RATE & REVIEW A BOOK\n";
    cout << "==================================\n";
    const int resourceID = InputHelper::readValidatedInt("Resource ID: ", Validator::validateID);
    
    Resource* res = lib.searchByID(resourceID);
    if (!res) {
        cout << "Resource not found.\n";
        return;
    }
    
    cout << "\nBook: " << res->getTitle() << " by " << res->getAuthor() << "\n";
    cout << "Category: " << res->getCategory() << "\n";
    int rating = InputHelper::readMenuChoice("Rating (1-5): ", 1, 5);
    
    if (InputHelper::askConfirmation("Would you like to add a review?")) {
        const string reviewText = InputHelper::readValidatedLine("Review (max 200 chars): ", Validator::validateString);
        
        lib.addRating(resourceID, member->getUserID(), rating, reviewText);
        cout << "\nReview submitted successfully! Thank you for sharing your feedback.\n";
    } else {
        lib.addRating(resourceID, member->getUserID(), rating);
        cout << "\nRating submitted successfully!\n";
    }
}

void extendBorrowPeriod(LibraryManager& lib, Member* member)
{
    if (!member)
        throw AuthorizationException("Only members can extend borrow periods.");

    cout << "\n==================================\n";
    cout << "   EXTEND BORROW PERIOD\n";
    cout << "==================================\n";
    const int resourceID = InputHelper::readValidatedInt("Resource ID to extend: ", Validator::validateID);
    
    auto borrowedBooks = member->getBorrowedBooks();
    bool found = false;
    for (const auto& book : borrowedBooks) {
        if (book.resourceID == resourceID) {
            found = true;
            cout << "Current Due Date: " << book.dueDate << "\n";
            int days = InputHelper::readMenuChoice("Days (1-14): ", 1, 14);
            if (days < 1 || days > 14) {
                throw ValidationException("Extension days must be between 1 and 14 days.");
            }
            
            lib.requestExtension(member->getUserID(), resourceID, book.dueDate, days);
            cout << "\nExtension Request Submitted!\n";
            cout << "Status: Pending Admin Approval\n";
            cout << "You will be notified when the decision is made.\n";
            break;
        }
    }
    
    if (!found) {
        cout << "You have not borrowed this resource.\n";
    }
}

// Include the rest of the functions from the original main.cpp
// viewAdvancedSearch, renewMembership, payFines, handleMemberAction, handleAdminAction exactly as in original

void viewAdvancedSearch(LibraryManager& lib, User* user = nullptr)
{
    cout << "\n==================================\n";
    cout << "    ADVANCED SEARCH\n";
    cout << "==================================\n";
    cout << "1. Search by Title\n";
    cout << "2. Search by Author\n";
    cout << "3. Search by Category\n";
    cout << "4. Search by ISBN\n";
    cout << "5. View Unavailable Books & Request to Borrow\n";
    cout << "0. Back\n";
    
    int searchChoice = InputHelper::readMenuChoice("Search Option: ", 0, 5);
    
    vector<Resource*> results;
    
    if (searchChoice == 1) {
        string title = InputHelper::readValidatedLine("Title: ", Validator::validateString);
        results = AdvancedSearch::searchByTitle(lib.getResources(), title);
    } else if (searchChoice == 2) {
        string author = InputHelper::readValidatedLine("Author: ", Validator::validateName);
        results = AdvancedSearch::searchByAuthor(lib.getResources(), author);
    } else if (searchChoice == 3) {
        string category = InputHelper::readValidatedLine("Category: ", Validator::validateString);
        results = AdvancedSearch::searchByCategory(lib.getResources(), category);
    } else if (searchChoice == 4) {
        string isbn = InputHelper::readValidatedLine("ISBN: ", Validator::validateISBN);
        results = AdvancedSearch::searchByISBN(lib.getResources(), isbn);
    } else if (searchChoice == 5) {
        results = AdvancedSearch::searchUnavailable(lib.getResources());
        cout << "Unavailable Resources (Request to Borrow)\n";
    }
    
    if (results.empty()) {
        cout << "No results found.\n";
        return;
    }
    
    cout << "\n=== SEARCH RESULTS (" << results.size() << ") ===\n";
    for (size_t i = 0; i < results.size(); i++) {
        cout << "\n[" << (i + 1) << "] ";
        results[i]->displayDetails();
    }
    
    // For unavailable books, allow requesting to borrow
    if (searchChoice == 5 && user && dynamic_cast<Member*>(user)) {
        cout << "\n>>> REQUEST TO BORROW <<<\n";
        cout << "Enter book number to request (0 to skip): ";
        int requestChoice = InputHelper::readValidatedInt("", Validator::validatePositiveOrZero);
        
        if (requestChoice > 0 && requestChoice <= (int)results.size()) {
            int resourceID = results[requestChoice - 1]->getResourceID();
            Member* member = dynamic_cast<Member*>(user);
            
            if (member->findBorrowedBook(resourceID)) {
                cout << "You have already borrowed this book.\n";
            } else if (lib.isInWaitlist(user->getUserID(), resourceID)) {
                cout << "You are already in the waitlist for this book.\n";
            } else {
                lib.addToWaitlist(user->getUserID(), resourceID);
                string notifMessage = "You have been added to the waitlist for Resource ID " + to_string(resourceID) + " (" + results[requestChoice - 1]->getTitle() + "). You will be notified when it becomes available.";
                lib.sendNotification(user->getUserID(), resourceID, notifMessage, NotificationType::BOOK_AVAILABLE);
                cout << "Request submitted! You will be notified when the book becomes available.\n";
            }
        }
    }
}

void renewMembership(UserManager& um, Member* member)
{
    if (!member)
        throw AuthorizationException("Only members can renew membership.");

    cout << "\n==================================\n";
    cout << "   RENEW MEMBERSHIP\n";
    cout << "==================================\n";
    cout << "Current Type: " << member->getMembershipTypeName() << "\n";
    cout << "Current Expiry: " << member->getMembershipExpiry() << "\n";
    cout << "1. Silver  - 2 books, 14 days\n";
    cout << "2. Gold    - 5 books, 21 days\n";
    cout << "3. Platinum- 10 books, 30 days\n";

    const int typeChoice = InputHelper::readMenuChoice("Membership type: ", 1, 3);
    const int months = InputHelper::readMenuChoice("Renew for months (1-12): ", 1, 12);

    member->setMembershipType(static_cast<MembershipType>(typeChoice - 1));
    member->extendMembership(months);
    um.saveToFile();

    cout << "Membership renewed. New expiry: " << member->getMembershipExpiry() << "\n";
}

void payFines(UserManager& um, Member* member)
{
    if (!member)
        throw AuthorizationException("Only members can pay fines.");

    const double balance = member->getBalance();
    if (balance >= 0.0) {
        cout << "No outstanding fines.\n";
        return;
    }

    cout << "\nOutstanding fine: Rs. " << -balance << "\n";
    const double amount = readValidatedAmount("Payment amount: Rs. ");

    if (amount > -balance)
        throw BalanceException("Payment cannot be greater than the outstanding fine.");

    member->updateBalance(amount);
    um.saveToFile();

    cout << "Payment recorded. Remaining fine: Rs. " << (-member->getBalance()) << "\n";
}

void depositFunds(UserManager& um, Member* member)
{
    if (!member)
        throw AuthorizationException("Only members can deposit funds.");

    const double amount = readValidatedAmount("Deposit amount (Rs.): ");
    if (amount > 5000.0) {
        throw ValidationException("Deposit amount cannot be greater than Rs. 5000.");
    }

    member->updateBalance(amount);
    um.saveToFile();

    if (member->getBalance() > 0) {
        cout << "\nFunds added successfully. Current account credit: Rs. " << member->getBalance() << "\n";
    } else if (member->getBalance() < 0) {
        cout << "Funds added. Outstanding fines after deposit: Rs. " << -member->getBalance() << "\n";
    } else {
        cout << "Funds added. Current balance: Rs. 0\n";
    }
}

void updateResourceFromInput(LibraryManager& lib)
{
    cout << "\n==================================\n";
    cout << "   UPDATE RESOURCE DETAILS\n";
    cout << "==================================\n";
    const int resourceID = InputHelper::readValidatedInt("Resource ID to update: ", Validator::validateID);
    Resource* resource = lib.searchByID(resourceID);
    if (!resource) {
        cout << "Resource not found.\n";
        return;
    }

    cout << "\nCurrent resource details:\n";
    resource->displayDetails();
    cout << "\nPress ENTER to keep a field unchanged.\n";

    string title = InputHelper::readLine("Title [" + resource->getTitle() + "]: ");
    if (!title.empty())
        resource->setTitle(title);

    string author = InputHelper::readLine("Author [" + resource->getAuthor() + "]: ");
    if (!author.empty())
        resource->setAuthor(author);

    string category = InputHelper::readLine("Category [" + resource->getCategory() + "]: ");
    if (!category.empty())
        resource->setCategory(category);

    bool availability = InputHelper::askConfirmation("Is the resource currently available?");
    resource->setAvailability(availability);

    if (auto* book = dynamic_cast<Book*>(resource)) {
        string isbn = InputHelper::readLine("ISBN [" + book->getISBN() + "]: ");
        if (!isbn.empty())
            book->setISBN(isbn);

        string publisher = InputHelper::readLine("Publisher [" + book->getPublisher() + "]: ");
        if (!publisher.empty())
            book->setPublisher(publisher);

        string pagesStr = InputHelper::readLine("Pages [" + to_string(book->getPages()) + "]: ");
        if (!pagesStr.empty()) {
            int pages = Validator::parseInt(pagesStr);
            book->setPages(pages);
        }
    }
    else if (auto* ebook = dynamic_cast<EBook*>(resource)) {
        string fileSizeStr = InputHelper::readLine("File size in MB [" + to_string(ebook->getFileSize()) + "]: ");
        if (!fileSizeStr.empty()) {
            float fileSize = stof(fileSizeStr);
            ebook->setFileSize(fileSize);
        }

        string format = InputHelper::readLine("Format [" + ebook->getFormat() + "]: ");
        if (!format.empty())
            ebook->setFormat(format);

        string link = InputHelper::readLine("Download link [" + ebook->getDownloadLink() + "]: ");
        if (!link.empty())
            ebook->setDownloadLink(link);
    }

    lib.saveResourcesToFile();
    cout << "Resource updated successfully.\n";
}

bool askReturnToPreviousMenu()
{
    cout << "\nPress Enter to return to the previous menu...\n";
    InputHelper::readLine("");
    return true;
}

void handleMemberAction(LibraryManager& lib, UserManager& um, User* user, int choice)
{
    if (choice == 1) {
        user->displayProfile();
        return;
    }

    if (choice == 2) {
        vector<string> categories = lib.getMacroCategories();
        
        if (categories.empty()) {
            cout << "No categories available.\n";
            return;
        }
        
        while (true) {
            cout << "\n==================================\n";
            cout << "    BROWSE BY CATEGORY\n";
            cout << "==================================\n";
            for (size_t i = 0; i < categories.size(); i++) {
                cout << (i + 1) << ". " << categories[i] << "\n";
            }
            cout << "0. Back to Menu\n";
            
            int catChoice = InputHelper::readValidatedInt("Select Your Desired Option: ", Validator::validateMenuChoiceWithZero);
            
            if (catChoice == 0) return;
            if (catChoice > 0 && catChoice <= (int)categories.size()) {
                string selectedCategory = categories[catChoice - 1];
                vector<Resource*> categoryResults;
                set<string> allDetailedCats = lib.getAllCategories();
                for (const auto& detailedCat : allDetailedCats) {
                    if (Validator::getMacroCategory(detailedCat) == selectedCategory) {
                        vector<Resource*> matches = lib.searchByCategory(detailedCat);
                        for (auto* res : matches) {
                            if (!res || !res->checkAvailability())
                                continue;

                            if (lib.isReserved(res->getResourceID()) && lib.getReservationHolder(res->getResourceID()) != user->getUserID())
                                continue;

                            categoryResults.push_back(res);
                        }
                    }
                }
                
                if (categoryResults.empty()) {
                    cout << "No available books in this category right now.\n";
                    continue;
                }
                
                for (size_t i = 0; i < categoryResults.size(); i++) {
                    cout << "\n[" << (i + 1) << "] ";
                    categoryResults[i]->displayDetails();
                    cout << "----------------------\n";
                }
                
                // Offer to borrow
                cout << "\n>>> BORROW OPTIONS <<<\n";
                cout << "Enter book number to borrow (0 to skip): ";
                int borrowChoice = InputHelper::readValidatedInt("", Validator::validatePositiveOrZero);
                
                if (borrowChoice > 0 && borrowChoice <= (int)categoryResults.size()) {
                    int resourceID = categoryResults[borrowChoice - 1]->getResourceID();
                    Member* member = dynamic_cast<Member*>(user);
                    if (member && member->findBorrowedBook(resourceID)) {
                        cout << "You have already borrowed this book.\n";
                    } else if (member) {
                        Resource* res = lib.searchByID(resourceID);
                        if (!res || !res->checkAvailability()) {
                            cout << "\n>>> NOT AVAILABLE <<<\n";
                            cout << "This book is currently borrowed by another member.\n";
                            cout << "You cannot borrow it at this time.\n";
                            if (InputHelper::askConfirmation("Would you like to be added to the waitlist for this book?")) {
                                lib.addToWaitlist(user->getUserID(), resourceID);
                                string notifMessage = "You have been added to the waitlist for Resource ID " + to_string(resourceID) + " (" + res->getTitle() + "). You will be notified when it is returned.";
                                lib.sendNotification(user->getUserID(), resourceID, notifMessage, NotificationType::BOOK_AVAILABLE);
                                cout << "You have been added to the waitlist.\n";
                            }
                            cout << "A notification has been added to your notifications tab.\n";
                            return;
                        }
                        
                        cout << "\n>>> BORROW CONFIRMATION <<<\n";
                        res->displayDetails();
                        
                        if (!InputHelper::askConfirmation("Are you sure you want to borrow this book?")) {
                            cout << "Borrow cancelled.\n";
                            return;
                        }
                        
                        string issueDate = InputHelper::getCurrentDate();
                        string dueDate = InputHelper::getDueDate(member->getBorrowDays());
                        
                        cout << "\n>>> BORROW DETAILS <<<\n";
                        cout << "Issue Date: " << issueDate << "\n";
                        cout << "Due Date: " << dueDate << "\n";
                        
                        try {
                            lib.borrowResource(user->getUserID(), resourceID, issueDate, dueDate);
                            cout << "\nBorrow processed successfully!\n";
                            cout << "   A notification has been sent to your notifications tab.\n";
                        } catch (const exception& e) {
                            cout << "Error: " << e.what() << "\n";
                        }
                    }
                }
            }
        }
        return;
    }

    if (choice == 3) {
        viewAdvancedSearch(lib, user);
        return;
    }

    if (choice == 4) {
        const int resourceID = InputHelper::readValidatedInt("Physical Resource ID to borrow: ", Validator::validateID);
        
        Resource* res = lib.searchByID(resourceID);
        if (!res) {
            cout << "Resource not found.\n";
            return;
        }
        
        Member* member = dynamic_cast<Member*>(user);
        if (member && member->findBorrowedBook(resourceID)) {
            cout << "You have already borrowed this book.\n";
            return;
        }
        
        if (!res->checkAvailability()) {
            cout << "\n>>> NOT AVAILABLE <<<\n";
            cout << "This book is currently borrowed by another member.\n";
            cout << "You cannot borrow it at this time.\n";
            
            if (InputHelper::askConfirmation("Would you like to be added to the waitlist for this book?")) {
                lib.addToWaitlist(user->getUserID(), resourceID);
                string notifMessage = "You have been added to the waitlist for Resource ID " + to_string(resourceID) + " (" + res->getTitle() + "). You will be notified when it is returned.";
                lib.sendNotification(user->getUserID(), resourceID, notifMessage, NotificationType::BOOK_AVAILABLE);
                cout << "You have been added to the waitlist.\n";
            }
            cout << "A notification has been added to your notifications tab.\n";
            return;
        }

        if (lib.isReserved(resourceID) && lib.getReservationHolder(resourceID) != user->getUserID()) {
            cout << "\n>>> RESERVED <<<\n";
            cout << "This resource is reserved by another member. Please choose a different resource or wait until it becomes available.\n";
            return;
        }
        
        cout << "\n>>> BORROW CONFIRMATION <<<\n";
        res->displayDetails();
        
        if (!InputHelper::askConfirmation("Are you sure you want to borrow this book?")) {
            cout << "Borrow cancelled.\n";
            return;
        }
        
        string issueDate = InputHelper::getCurrentDate();
        string dueDate = InputHelper::getDueDate(member->getBorrowDays());
        
        cout << "\n>>> BORROW DETAILS <<<\n";
        cout << "Issue Date: " << issueDate << "\n";
        cout << "Due Date: " << dueDate << "\n";
        
        try {
            lib.borrowResource(user->getUserID(), resourceID, issueDate, dueDate);
            cout << "\nBorrow processed successfully!\n";
            cout << "A notification has been sent to your notifications tab.\n";
        }
        catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
        return;
    }

    if (choice == 5) {
        const int resourceID = InputHelper::readValidatedInt("Digital Resource ID to access: ", Validator::validateID);
        Resource* res = lib.searchByID(resourceID);
        if (!res) {
            cout << "Resource not found.\n";
            return;
        }

        if (!res->isDigital()) {
            cout << "This resource is not available as a digital item. Please use Borrow Physical Resource.\n";
            return;
        }

        Member* member = dynamic_cast<Member*>(user);
        if (member && member->findBorrowedBook(resourceID)) {
            cout << "You already have access to this resource.\n";
            return;
        }

        if (!res->checkAvailability()) {
            cout << "\n>>> NOT AVAILABLE <<<\n";
            cout << "This digital resource currently has no available licenses.\n";
            return;
        }

        cout << "\n>>> DIGITAL ACCESS CONFIRMATION <<<\n";
        res->displayDetails();

        if (!InputHelper::askConfirmation("Would you like to access this digital resource now?")) {
            cout << "Access cancelled.\n";
            return;
        }

        string issueDate = InputHelper::getCurrentDate();
        string dueDate = InputHelper::getDueDate(member->getBorrowDays());

        cout << "\n>>> ACCESS DETAILS <<<\n";
        cout << "Start Date: " << issueDate << "\n";
        cout << "Access Expiry: " << dueDate << "\n";

        try {
            lib.borrowResource(user->getUserID(), resourceID, issueDate, dueDate);
            cout << "\nDigital access granted successfully!\n";
            if (auto ebook = dynamic_cast<EBook*>(res)) {
                cout << "Access Link: " << ebook->getDownloadLink() << "\n";
            }
        }
        catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
        return;
    }

    if (choice == 6) {
        const int resourceID = InputHelper::readValidatedInt("Resource ID to return: ", Validator::validateID);
        if (!InputHelper::askConfirmation("Confirm return now?")) {
            cout << "Return cancelled.\n";
            return;
        }

        const string returnDate = InputHelper::getCurrentDate();
        try {
            lib.returnResource(user->getUserID(), resourceID, returnDate);
            cout << "Return recorded. Admin will verify the resource condition and apply any damage fees if needed.\n";
        }
        catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
        return;
    }

    if (choice == 7) {
        extendBorrowPeriod(lib, dynamic_cast<Member*>(user));
        return;
    }

    if (choice == 8) {
        lib.viewBorrowHistory(user->getUserID());
        return;
    }

    if (choice == 9) {
        rateBook(lib, dynamic_cast<Member*>(user));
        return;
    }

    if (choice == 10) {
        Member* member = dynamic_cast<Member*>(user);
        if (member) {
            vector<Notification*> notifs = lib.getUserNotifications(user->getUserID());
            if (notifs.empty()) {
                cout << "No notifications.\n";
                return;
            }
            
            cout << "\n==================================\n";
            cout << "     YOUR NOTIFICATIONS             \n";
            cout << "==================================\n";
            for (size_t i = 0; i < notifs.size(); i++) {
                cout << "\n[" << (i + 1) << "] ";
                notifs[i]->display();
                lib.markNotificationAsRead(user->getUserID(), notifs[i]->getNotificationID());
                cout << "----------------------\n";
            }
            um.saveToFile();
        }
        return;
    }

    if (choice == 11) {
        renewMembership(um, dynamic_cast<Member*>(user));
        return;
    }

    if (choice == 12) {
        depositFunds(um, dynamic_cast<Member*>(user));
        return;
    }

    if (choice == 13) {
        const int resourceID = InputHelper::readValidatedInt("Resource ID to reserve: ", Validator::validateID);
        Resource* res = lib.searchByID(resourceID);
        if (!res) {
            cout << "Resource not found.\n";
            return;
        }

        if (!res->checkAvailability()) {
            cout << "This resource is currently unavailable.\n";
            if (InputHelper::askConfirmation("Would you like to be added to the waitlist for this book?")) {
                lib.addToWaitlist(user->getUserID(), resourceID);
                string notifMessage = "You have been added to the waitlist for Resource ID " + to_string(resourceID) + " (" + res->getTitle() + "). You will be notified when it becomes available.";
                lib.sendNotification(user->getUserID(), resourceID, notifMessage, NotificationType::BOOK_AVAILABLE);
                cout << "You have been added to the waitlist.\n";
            }
            return;
        }

        cout << "\n=== RESOURCE DETAILS ===\n";
        res->displayDetails();

        if (!InputHelper::askConfirmation("Confirm reservation for this resource?")) {
            cout << "Reservation cancelled.\n";
            return;
        }

        try {
            lib.reserveResource(user->getUserID(), resourceID);
            cout << "Resource reserved successfully. You can borrow it later from your account.\n";
        }
        catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
        return;
    }
}

void handleAdminAction(LibraryManager& lib, UserManager& um, int choice)
{
    if (choice == 1) {
        um.displayAllMembers();
        return;
    }

    if (choice == 2) {
        lib.displayAllResources();
        return;
    }

    if (choice == 4) {
        const int userID = InputHelper::readValidatedInt("Member User ID: ", Validator::validateID);
        const int resourceID = InputHelper::readValidatedInt("Resource ID: ", Validator::validateID);
        const string issueDate = InputHelper::readValidatedLine("Issue Date (YYYY-MM-DD): ", Validator::validateDate);
        const string dueDate = InputHelper::readValidatedLine("Due Date (YYYY-MM-DD): ", Validator::validateDate);
        
        try {
            lib.borrowResource(userID, resourceID, issueDate, dueDate);
            cout << "\nBorrow processed successfully.\n";
        }
        catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
        }
        return;
    }

    if (choice == 5) {
        const int userID = InputHelper::readValidatedInt("Member User ID: ", Validator::validateID);
        const int resourceID = InputHelper::readValidatedInt("Resource ID: ", Validator::validateID);
        const string returnDate = InputHelper::readValidatedLine("Return Date (YYYY-MM-DD): ", Validator::validateDate);
        
        try {
            lib.returnResource(userID, resourceID, returnDate);
            Resource* res = lib.searchByID(resourceID);
            if (res) {
                cout << "\n=== RETURNED RESOURCE DETAILS ===\n";
                res->displayDetails();
            }

            BorrowRecord* rec = lib.findRecord(userID, resourceID);
            if (rec) {
                double fine = rec->getFineAmount();
                cout << "\n>>> RETURN VERIFICATION <<<\n";
                cout << "Return Date : " << returnDate << "\n";
                cout << "Due Date    : " << rec->getDueDate() << "\n";
                cout << "Fine        : $" << fine << "\n";
            }

            if (InputHelper::askConfirmation("Is the returned resource damaged?")) {
                double damageFee = readValidatedAmount("Damage fee amount (Rs.): ");
                lib.applyDamageFee(userID, resourceID, damageFee);
                cout << "Damage fee recorded successfully.\n";
            }
        }
        catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
        }
        return;
    }
}

}  // end namespace

int main() {
    try {
        UserManager um("users.txt");
        ReportGenerator rg;
        LibraryManager lib(um, rg);

        int choice;

        // System initialization: require super admin to be registered first
        if (!um.adminExists()) {
            cout << "\n";
            cout << "======================================================\n";
            cout << "              SYSTEM INITIALIZATION REQUIRED             \n";
            cout << "               LIBRARY MANAGEMENT SYSTEM                \n";
            cout << "=======================================================\n";
            cout << "\nWelcome! This library system requires a Super Admin to be\n";
            cout << "registered as the first step before anyone else can use it.\n\n";
            cout << "IMPORTANT: Only ONE Super Admin is allowed in the entire system.\n";
            cout << "After you register as Super Admin, you can create other admins\n";
            cout << "with different roles (Librarian, Fine Manager, etc.)\n\n";

            bool initialized = false;
            while (!initialized) {
                if (!InputHelper::askConfirmation("Would you like to register as the Super Admin now?")) {
                    cout << "\nSystem cannot proceed without a Super Admin.\n";
                    cout << "Please restart the application and register as Super Admin.\n";
                    return 0;
                }

                try {
                    um.registerAdmin(true);  // Force super admin role
                    cout << "\n System initialization complete!\n";
                    cout << "You can now log in and manage the library system.\n";
                    initialized = true;
                }
                catch (const exception& e) {
                    cout << "Error during super admin registration: " << e.what() << endl;
                    if (!InputHelper::askRetryOrExit()) {
                        cout << "\nSystem cannot proceed without a valid Super Admin registration.\n";
                        return 0;
                    }
                }
            }
        }

        while (true) {
            cout << "\n";
            cout << "======================================================\n";
            cout << "               LIBRARY MANAGEMENT SYSTEM                \n";
            cout << "               Central Library Portal                   \n";
            cout << " ======================================================\n";
            cout << "\nPlease select an option to continue:\n";
            cout << "  1. Register as a Member\n";
            cout << "  2. Register as an Admin (Super Admin use only)\n";
            cout << "  3. Login to your account\n";
            cout << "  0. Exit\n";
            cout << "\n=======================================================\n";
            try {
                choice = InputHelper::readMenuChoice("Enter your choice: ", 0, 3);
            }
            catch (const exception& e) {
                if (dynamic_cast<const OperationCancelledException*>(&e) != nullptr)
                    break;
                throw;
            }

            if (choice == 0) break;

            try {
                if (choice == 1) {
                    um.registerMember();
                }
                else if (choice == 2) {
                    cout << "\n======================================================\n";
                    cout << "    ADMIN REGISTRATION RESTRICTED       \n";
                    cout << "======================================================\n";
                    cout << "Only SUPER ADMINS can register other admins.\n";
                    cout << "If you need to register a new admin, please:\n";
                    cout << "  1. Log in as the Super Admin\n";
                    cout << "  2. Navigate to Member Management\n";
                    cout << "  3. Use the admin registration option\n\n";
                    cout << "This restriction ensures only authorized personnel\n";
                    cout << "can grant admin privileges in the system.\n";
                }
                else if (choice == 3) {
                    User* user = nullptr;

                    while (true) {
                        const string username = InputHelper::readValidatedLine("\nUsername: ", Validator::validateUsername);
                        const string password = InputHelper::readValidatedLine("Password: ", Validator::validatePassword);

                        try {
                            User* tempUser = um.findUserByUsername(username);
                            if (tempUser) {
                                Member* member = dynamic_cast<Member*>(tempUser);
                                if (member && member->isLocked()) {
                                    cout << "\n ACCOUNT LOCKED\n";
                                    cout << "Your account has been locked due to multiple failed login attempts.\n";
                                    cout << "Lockout ends: " << member->getLockoutEndTime() << "\n";
                                    cout << "Please try again later or contact library support.\n";
                                    continue;
                                }
                            }
                            
                            user = um.loginUser(username, password);
                            
                            if (user) {
                                Member* member = dynamic_cast<Member*>(user);
                                if (member) {
                                    member->resetFailedLogin();
                                }
                            }
                            break;
                        }
                        catch (const exception& e) {
                            cout << "Error: " << e.what() << endl;
                            
                            User* failedUser = um.findUserByUsername(username);
                            if (failedUser) {
                                Member* member = dynamic_cast<Member*>(failedUser);
                                if (member) {
                                    member->incrementFailedLogin();
                                    int attempts = member->getFailedLoginAttempts();
                                    if (attempts >= 3) {
                                        cout << "\n  WARNING: " << attempts << " failed attempts.\n";
                                        cout << "After 5 failed attempts, your account will be locked for 30 minutes.\n";
                                    }
                                }
                            }

                            if (!InputHelper::askRetryOrExit())
                                throw OperationCancelledException();
                        }
                    }

                    cout << "\nLogin successful! Welcome, " << user->getFirstName() << " " << user->getLastName() << "!\n";
                    
                    if (user->getRole() == "Member") {
                        Member* member = dynamic_cast<Member*>(user);
                        if (member) {
                            cout << "\n--- Membership Info ---\n";
                            cout << "Type: " << member->getMembershipTypeName() << " | ";
                            cout << "Borrow Limit: " << member->getBorrowedCount() << "/" << member->getMaxBorrowLimit() << " | ";
                            cout << "Expiry: " << member->getMembershipExpiry() << "\n";
                            if (member->getBalance() > 0) {
                                cout << "  Account Credit: Rs. " << member->getBalance() << "\n";
                            } else if (member->getBalance() < 0) {
                                cout << "  Outstanding Fines: Rs. " << fabs(member->getBalance()) << "\n";
                            }
                        }
                    }

                    if (user->getRole() == "Member") {
                        int mChoice;
                        do {
                            Member* member = dynamic_cast<Member*>(user);
                            cout << "\n";
                            cout << "======================================================\n";
                            cout << "            MEMBER DASHBOARD               \n";
                            cout << " ======================================================\n";
                            cout << "  Welcome back, " << user->getFirstName() << " " << user->getLastName() << "!\n";
                            if (member) {
                                cout << "  Membership: " << member->getMembershipTypeName() << "\n";
                                cout << "  Borrowed Books: " << member->getBorrowedCount() << "/" << member->getMaxBorrowLimit() << "\n";
                                double balance = member->getBalance();
                                cout << "  Account Balance: Rs. ";
                                if (balance > 0) {
                                    cout << balance << " (Credit)";
                                } else if (balance < 0) {
                                    cout << fabs(balance) << " (Outstanding Fines)";
                                } else {
                                    cout << "0.00";
                                }
                                cout << "\n";
                            }
                            cout << "\n=========================================\n";
                            cout << "  RESOURCE MANAGEMENT\n";
                            cout << "  1. View Profile\n";
                            cout << "  2. Browse Available Resources\n";
                            cout << "  3. Search Resources\n";
                            cout << "  4. Borrow Physical Resource\n";
                            cout << "  5. Access Digital Resource\n";
                            cout << "  6. Return Resource\n";
                            cout << "  7. Extend Due Date\n";
                            cout << "  8. My Borrow History\n";
                            cout << "  9. Rate & Review Books\n";
                            cout << "  10. Notifications";
                            if (member && member->getUnreadNotificationCount() > 0) {
                                cout << " (" << member->getUnreadNotificationCount() << " new)";
                            }
                            cout << "\n\n  ACCOUNT MANAGEMENT\n";
                            cout << "  11. Renew Membership\n";
                            cout << "  12. Add Funds\n";
                            cout << "  13. Reserve Resource\n";
                            cout << "\n  0. Logout\n";
                            cout << "=========================================\n";
                            mChoice = InputHelper::readMenuChoice("Enter your choice: ", 0, 13);

                            if (mChoice != 0) {
                                try {
                                    handleMemberAction(lib, um, user, mChoice);
                                }
                                catch (const OperationCancelledException&) {
                                    cout << "Operation cancelled.\n";
                                }
                                catch (const exception& e) {
                                    cout << "Error: " << e.what() << endl;
                                }
                            }

                        } while (mChoice != 0);
                    }
                    else if (user->getRole() == "Admin") {
                        Admin* admin = dynamic_cast<Admin*>(user);
                        bool isSuperAdmin = admin && admin->hasSuperAdminPrivilege();
                        bool canManageResources = admin && admin->canManageResources();
                        bool canManageNotifications = admin && admin->canManageNotifications();
                        bool canManageFines = admin && admin->canManageFines();
                        bool canManageMembers = admin && admin->canManageMembers();

                        int aChoice;
                        do {
                            cout << "\n";
                            cout << "======================================================\n";
                            cout << "            ADMIN DASHBOARD               \n";
                            cout << " ======================================================\n";
                            if (isSuperAdmin) {
                                cout << "  [SUPER ADMIN MODE]\n";
                            } else if (admin) {
                                cout << "  [" << admin->getAdminRoleName() << " MODE]\n";
                            }
                            cout << "\n========================================\n";
                            cout << "1. View My Profile\n";
                            if (canManageMembers) cout << "2. View All Members\n";
                            cout << "3. View All Resources\n";
                            cout << "4. Process Borrow Request\n";
                            cout << "5. Process Return Request\n";
                            cout << "6. View Pending Requests\n";
                            cout << "7. View Member Borrow History\n";
                            cout << "8. Generate All Member Borrow Reports\n";
                            cout << "9. Generate Issued/Overdue Resource Report\n";
                            if (canManageNotifications) cout << "10. Notification Management\n";
                            if (canManageResources) cout << "11. Resource Management\n";
                            if (canManageFines) cout << "12. Fine Management\n";
                            if (isSuperAdmin) {
                                cout << "13. Member Management\n";
                                cout << "14. Analytics Dashboard\n";
                                cout << "15. View Audit Log\n";
                                cout << "16. System Statistics\n";
                                cout << "17. Admin Management\n";
                            }
                            cout << "0. Logout\n";
                            
                            int maxChoice = 9;
                            if (canManageNotifications) maxChoice = 10;
                            if (canManageResources) maxChoice = 11;
                            if (canManageFines) maxChoice = 12;
                            if (isSuperAdmin) maxChoice = 17;
                            aChoice = InputHelper::readMenuChoice("Choice: ", 0, maxChoice);

                            if (aChoice != 0) {
                                try {
                                    if (aChoice == 1) {
                                        if (admin) admin->displayFullProfile();
                                    }
                                    else if (aChoice == 2 && canManageMembers) {
                                        um.displayAllMembers();
                                    }
                                    else if (aChoice == 3) {
                                        lib.displayAllResources();
                                    }
                                    else if (aChoice >= 4 && aChoice <= 5) {
                                        handleAdminAction(lib, um, aChoice);
                                    }
                                    else if (aChoice == 6) {
                                        cout << "\n======================================================\n";
                                        cout << "     PENDING REQUESTS                  \n";
                                        cout << " ======================================================\n";
                                        bool found = false;
                                        auto pending = lib.getPendingRequests();
                                        if (pending.empty()) {
                                            cout << "No pending requests.\n";
                                        }
                                        else {
                                            for (const auto& req : pending) {
                                                cout << "Request ID: " << req.first << " - User: " << req.second << "\n";
                                                found = true;
                                            }
                                        }
                                        if (!found) cout << "No pending requests.\n";
                                    }
                                    else if (aChoice == 7) {
                                        int userID = InputHelper::readValidatedInt("Member User ID: ", Validator::validateID);
                                        lib.viewBorrowHistory(userID);
                                    }
                                    else if (aChoice == 8) {
                                        lib.displayAllMembersBorrowHistory();
                                    }
                                    else if (aChoice == 9) {
                                        lib.displayIssuedOrOverdueResourceReport();
                                    }
                                    else if (aChoice == 10 && canManageNotifications) {
                                        while (true) {
                                            cout << "\n======================================================\n";
                                            cout << "     NOTIFICATION MANAGEMENT          \n";
                                            cout << " ======================================================\n";
                                            cout << "1. Send Notification\n";
                                            cout << "2. View All Notifications\n";
                                            cout << "3. View Notifications For User\n";
                                            cout << "4. Clear Notifications For User\n";
                                            cout << "0. Back\n";
                                            int nChoice = InputHelper::readMenuChoice("Choice: ", 0, 4);
                                            if (nChoice == 0) break;
                                            if (nChoice == 1) {
                                                int userID = InputHelper::readValidatedInt("Target Member User ID: ", Validator::validateID);
                                                int resourceID = InputHelper::readValidatedInt("Resource ID: ", Validator::validateID);
                                                string message = InputHelper::readValidatedLine("Notification message: ", Validator::validateString);
                                                cout << "Select notification type:\n";
                                                cout << "1. BOOK_AVAILABLE\n";
                                                cout << "2. FINE_APPLIED\n";
                                                cout << "3. RETURN_CONFIRMED\n";
                                                cout << "4. BORROW_APPROVED\n";
                                                int typeChoice = InputHelper::readMenuChoice("Choice: ", 1, 4);
                                                NotificationType type = NotificationType::BOOK_AVAILABLE;
                                                if (typeChoice == 2) type = NotificationType::FINE_APPLIED;
                                                else if (typeChoice == 3) type = NotificationType::RETURN_CONFIRMED;
                                                else if (typeChoice == 4) type = NotificationType::BORROW_APPROVED;
                                                lib.sendNotification(userID, resourceID, message, type);
                                                cout << "Notification sent.\n";
                                            }
                                            else if (nChoice == 2) {
                                                lib.displayAllNotifications();
                                            }
                                            else if (nChoice == 3) {
                                                int userID = InputHelper::readValidatedInt("Member User ID: ", Validator::validateID);
                                                lib.displayUserNotifications(userID);
                                            }
                                            else if (nChoice == 4) {
                                                int userID = InputHelper::readValidatedInt("Member User ID: ", Validator::validateID);
                                                lib.clearNotifications(userID);
                                                cout << "Notifications cleared for user " << userID << ".\n";
                                            }
                                        }
                                    }
                                    else if (aChoice == 11 && canManageResources) {
                                        while (true) {
                                            cout << "\n======================================================\n";
                                            cout << "     RESOURCE MANAGEMENT             \n";
                                            cout << "======================================================\n";
                                            cout << "1. Add New Resource\n";
                                            cout << "2. Remove Resource\n";
                                            cout << "3. Update Resource\n";
                                            cout << "4. View All Resources\n";
                                            cout << "0. Back\n";
                                            int rmChoice = InputHelper::readMenuChoice("Choice: ", 0, 4);
                                            if (rmChoice == 0) break;
                                            if (rmChoice == 1) {
                                                addResourceFromInput(lib);
                                            }
                                            else if (rmChoice == 2) {
                                                int resID = InputHelper::readValidatedInt("Resource ID to remove: ", Validator::validateID);
                                                if (InputHelper::askConfirmation("Are you sure you want to remove this resource?")) {
                                                    lib.removeResource(resID);
                                                    cout << "Resource removed.\n";
                                                }
                                            }
                                            else if (rmChoice == 3) {
                                                updateResourceFromInput(lib);
                                            }
                                            else if (rmChoice == 4) {
                                                lib.displayAllResources();
                                            }
                                        }
                                    }
                                    else if (aChoice == 12 && canManageFines) {
                                        while (true) {
                                            cout << "\n======================================================\n";
                                            cout << "     FINE MANAGEMENT                 \n";
                                            cout << "======================================================\n";
                                            cout << "1. View All Member Fines\n";
                                            cout << "2. Adjust Member Balance\n";
                                            cout << "0. Back\n";
                                            int fmChoice = InputHelper::readMenuChoice("Choice: ", 0, 2);
                                            if (fmChoice == 0) break;
                                            if (fmChoice == 1) {
                                                bool found = false;
                                                for (const auto* u : um.getUsers()) {
                                                    const Member* member = dynamic_cast<const Member*>(u);
                                                    if (!member) continue;
                                                    if (member->getBalance() < 0) {
                                                        found = true;
                                                        cout << "User ID " << member->getUserID() << " | "
                                                             << member->getFirstName() << " " << member->getLastName()
                                                             << " | Outstanding Fine: Rs. " << -member->getBalance() << "\n";
                                                    }
                                                }
                                                if (!found) {
                                                    cout << "No members have outstanding fines at this time.\n";
                                                }
                                            }
                                            else if (fmChoice == 2) {
                                                int userID = InputHelper::readValidatedInt("Member User ID: ", Validator::validateID);
                                                string amountText = InputHelper::readLine("Adjust amount (+ credit, - fine): ");
                                                try {
                                                    double amount = stod(amountText);
                                                    um.adjustMemberBalance(userID, amount);
                                                }
                                                catch (const exception&) {
                                                    cout << "Invalid amount entered.\n";
                                                }
                                            }
                                        }
                                    }
                                    else if (aChoice == 13 && isSuperAdmin) {
                                        while (true) {
                                            cout << "\n======================================================\n";
                                            cout << "     MEMBER MANAGEMENT               \n";
                                            cout << "======================================================\n";
                                            cout << "1. View Member Details\n";
                                            cout << "2. Suspend Member\n";
                                            cout << "3. Reinstate Member\n";
                                            cout << "4. Delete Member\n";
                                            cout << "5. Edit Member\n";
                                            cout << "6. View Member History\n";
                                            cout << "0. Back\n";
                                            int mmChoice = InputHelper::readMenuChoice("Choice: ", 0, 6);
                                            if (mmChoice == 0) break;
                                            int userID = InputHelper::readValidatedInt("Member User ID: ", Validator::validateID);
                                            if (mmChoice == 1) {
                                                um.displayUserDetails(userID);
                                            }
                                            else if (mmChoice == 2) {
                                                if (InputHelper::askConfirmation("Are you sure you want to SUSPEND this member?")) {
                                                    um.suspendMember(userID);
                                                    cout << "Member suspended.\n";
                                                } else {
                                                    cout << "Action cancelled.\n";
                                                }
                                            }
                                            else if (mmChoice == 3) {
                                                um.reinstateMember(userID);
                                            }
                                            else if (mmChoice == 4) {
                                                if (InputHelper::askConfirmation("WARNING: This will PERMANENTLY delete the member. Are you sure?")) {
                                                    um.deleteMember(userID);
                                                    cout << "Member deleted.\n";
                                                } else {
                                                    cout << "Action cancelled.\n";
                                                }
                                            }
                                            else if (mmChoice == 5) {
                                                string field = InputHelper::readValidatedLine("Field (email/phone/address): ", Validator::validateString);
                                                string value = InputHelper::readValidatedLine("New value: ", Validator::validateString);
                                                um.editMember(userID, field, value);
                                            }
                                            else if (mmChoice == 6) {
                                                lib.viewBorrowHistory(userID);
                                            }
                                        }
                                    }
                                    else if (aChoice == 14 && isSuperAdmin) {
                                        AdminAnalytics analytics(&lib, &um);
                                        while (true) {
                                            cout << "\n======================================================\n";
                                            cout << "     ANALYTICS DASHBOARD              \n";
                                            cout << "======================================================\n";
                                            cout << "1. View Dashboard\n";
                                            cout << "2. Member Analytics\n";
                                            cout << "3. Book Analytics\n";
                                            cout << "4. Fine Analytics\n";
                                            cout << "5. Resource Analytics\n";
                                            cout << "0. Back\n";
                                            int anaChoice = InputHelper::readMenuChoice("Choice: ", 0, 5);
                                            if (anaChoice == 0) break;
                                            if (anaChoice == 1) {
                                                analytics.displayDashboard();
                                            } else if (anaChoice == 2) {
                                                analytics.displayMemberAnalytics();
                                            } else if (anaChoice == 3) {
                                                analytics.displayBookAnalytics();
                                            } else if (anaChoice == 4) {
                                                analytics.displayFineAnalytics();
                                            } else if (anaChoice == 5) {
                                                analytics.displayResourceAnalytics();
                                            }
                                        }
                                    }
                                    else if (aChoice == 15 && isSuperAdmin) {
                                        AuditLogger auditLog("audit_log.txt");
                                        cout << "\n=== Recent Audit Entries ===\n";
                                        auto entries = auditLog.getRecentEntries(20);
                                        auditLog.displayEntries(entries);
                                    }
                                    else if (aChoice == 16 && isSuperAdmin) {
                                        cout << "\n======================================================\n";
                                        cout << "     SYSTEM STATISTICS               \n";
                                        cout << "======================================================\n";
                                        cout << "Total Members: " << um.getMemberCount() << "\n";
                                        cout << "Total Resources: " << lib.getResources().size() << "\n";
                                        cout << "Active Borrows: " << lib.getActiveBorrows().size() << "\n";
                                    }
                                    else if (aChoice == 17 && isSuperAdmin) {
                                        while (true) {
                                            cout << "\n======================================================\n";
                                            cout << "     ADMIN MANAGEMENT                \n";
                                            cout << "======================================================\n";
                                            cout << "1. Register New Admin\n";
                                            cout << "2. View All Admins\n";
                                            cout << "3. Suspend Admin\n";
                                            cout << "4. Reinstate Admin\n";
                                            cout << "5. Delete Admin\n";
                                            cout << "0. Back\n";
                                            int amChoice = InputHelper::readMenuChoice("Choice: ", 0, 5);
                                            if (amChoice == 0) break;
                                            if (amChoice == 1) {
                                                um.registerAdmin(false);  // Allow normal admin registration from super admin
                                            }
                                            else if (amChoice == 2) {
                                                um.displayAllAdmins();
                                            }
                                            else if (amChoice > 0) {
                                                int adminID = InputHelper::readValidatedInt("Admin User ID: ", Validator::validateID);
                                                if (amChoice == 3) {
                                                    if (InputHelper::askConfirmation("Are you sure you want to SUSPEND this admin?")) {
                                                        um.suspendAdmin(adminID);
                                                        cout << "Admin suspended.\n";
                                                    } else {
                                                        cout << "Action cancelled.\n";
                                                    }
                                                }
                                                else if (amChoice == 4) {
                                                    um.reinstateAdmin(adminID);
                                                }
                                                else if (amChoice == 5) {
                                                    Admin* targetAdmin = um.findAdminByID(adminID);
                                                    if (targetAdmin && targetAdmin->hasSuperAdminPrivilege()) {
                                                        cout << "\nWARNING: You cannot delete the Super Admin!\n";
                                                        cout << "The system must have at least one Super Admin at all times.\n";
                                                    } else if (InputHelper::askConfirmation("WARNING: This will PERMANENTLY delete the admin. Are you sure?")) {
                                                        um.deleteAdmin(adminID);
                                                        cout << "Admin deleted.\n";
                                                    } else {
                                                        cout << "Action cancelled.\n";
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                catch (const OperationCancelledException&) {
                                    cout << "Operation cancelled.\n";
                                }
                                catch (const exception& e) {
                                    cout << "Error: " << e.what() << endl;
                                }
                            }
                        } while (aChoice != 0);
                    }
                }
            }
            catch (const exception& e) {
                if (dynamic_cast<const OperationCancelledException*>(&e) == nullptr)
                    cout << "Error: " << e.what() << endl;
            }
        }

        cout << "\nSystem shutting down...\n";
    }
    catch (const exception& e) {
        cout << "Fatal Error: " << e.what() << endl;
    }

    return 0;
}

