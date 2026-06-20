#include "Book.h"
#include "SYSTEMEXCEPTIONS.h"
#include <iostream>
#include <sstream>

using namespace std;

// ---------------- DEFAULT ----------------

Book::Book() {
    isbn_ = "";
    publisher_ = "";
    pages_ = 0;
}

// ---------------- PARAMETERIZED ----------------

Book::Book(int id, string t, string a, string c,
           bool status, string isbn, string publisher, int pages,
           string location)
    : Resource(id, t, a, c, status, ResourceMode::Physical, location)
{
    try {
        Validator::validateISBN(isbn);
        Validator::validateString(publisher);
        Validator::validatePages(pages);

        isbn_ = isbn;
        publisher_ = publisher;
        pages_ = pages;
    }
    catch (exception& e) {
        throw runtime_error("Book creation failed: " + string(e.what()));
    }
}

// ---------------- SETTERS ----------------

void Book::setISBN(string isbn) {
    Validator::validateISBN(isbn);
    isbn_ = isbn;
}

void Book::setPublisher(string publisher) {
    Validator::validateString(publisher);
    publisher_ = publisher;
}

void Book::setPages(int pages) {
    Validator::validatePages(pages);
    pages_ = pages;
}

// ---------------- GETTERS ----------------

string Book::getISBN() const {
    return isbn_;
}

string Book::getPublisher() const {
    return publisher_;
}

int Book::getPages() const {
    return pages_;
}

// ---------------- DISPLAY (POLYMORPHISM SAFE) ----------------

void Book::displayDetails() const {
    cout << "\n===================================\n";
    cout << "    PRINTED BOOK DETAILS\n";
    cout << "===================================\n";
    cout << "ID: " << getResourceID() << endl;
    cout << "Title: " << getTitle() << endl;
    cout << "Author: " << getAuthor() << endl;
    cout << "Category: " << getCategory() << endl;

    cout << "Status: "
         << (checkAvailability() ? "Available" : "Borrowed")
         << endl;

    cout << "ISBN: " << isbn_ << endl;
    cout << "Publisher: " << publisher_ << endl;
    cout << "Pages: " << pages_ << endl;
}

// ---------------- SERIALIZATION (FILE SAFE) ----------------

string Book::serialize() const {
    return to_string(getResourceID()) + "|" +
           "Book" + "|" +
           getTitle() + "|" +
           getAuthor() + "|" +
           getCategory() + "|" +
           (checkAvailability() ? "1" : "0") + "|" +
           isbn_ + "|" +
           publisher_ + "|" +
           to_string(pages_) + "|" +
           getLocation();
}

Book* Book::deserialize(const string& line) {
    stringstream ss(line);
    string token;

    getline(ss, token, '|');
    const int id = stoi(token);

    getline(ss, token, '|');
    if (token != "Book")
        throw ValidationException("Invalid resource type for book record.");

    string title;
    string author;
    string category;
    string isbn;
    string publisher;

    getline(ss, title, '|');
    getline(ss, author, '|');
    getline(ss, category, '|');
    getline(ss, token, '|');
    const bool isAvailable = (token == "1");
    getline(ss, isbn, '|');
    getline(ss, publisher, '|');
    getline(ss, token, '|');
    const int pages = stoi(token);
    string location;
    if (!getline(ss, location, '|'))
        location = "";

    return new Book(id, title, author, category, isAvailable, isbn, publisher, pages, location);
}
