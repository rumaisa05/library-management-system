#include "EBook.h"
#include "SYSTEMEXCEPTIONS.h"
#include <iostream>
#include <sstream>

using namespace std;

// ---------------- DEFAULT ----------------

EBook::EBook() {
    fileSize_ = 0.0;
    format_ = "";
    downloadLink_ = "";
    totalLicenses_ = 1;
    availableLicenses_ = 1;
    mode_ = ResourceMode::Digital;
    setAvailability(true);
}

// ---------------- PARAMETERIZED ----------------

EBook::EBook(int id, string t, string a, string c, bool status,
             float fileSize, string format, string link,
             int licenseCount, string location)
    : Resource(id, t, a, c, status, ResourceMode::Digital, location)
{
    try {
        Validator::validateFileSize(fileSize);
        Validator::validateFileFormat(format);
        Validator::validateUrl(link);
        if (licenseCount <= 0)
            throw ValidationException("License count must be at least 1.");

        fileSize_ = fileSize;
        format_ = format;
        downloadLink_ = link;
        totalLicenses_ = licenseCount;
        availableLicenses_ = licenseCount;
    }
    catch (exception& e) {
        throw runtime_error("EBook creation failed: " + string(e.what()));
    }
}

// ---------------- SETTERS ----------------

void EBook::setFileSize(float size) {
    Validator::validateFileSize(size);
    fileSize_ = size;
}

void EBook::setFormat(string format) {
    Validator::validateFileFormat(format);
    format_ = format;
}

void EBook::setDownloadLink(string link) {
    Validator::validateUrl(link);
    downloadLink_ = link;
}

void EBook::setLicenseCount(int count) {
    if (count <= 0)
        throw ValidationException("License count must be at least 1.");
    totalLicenses_ = count;
    if (availableLicenses_ > totalLicenses_)
        availableLicenses_ = totalLicenses_;
}

// ---------------- GETTERS ----------------

float EBook::getFileSize() const {
    return fileSize_;
}

string EBook::getFormat() const {
    return format_;
}

string EBook::getDownloadLink() const {
    return downloadLink_;
}

int EBook::getAvailableLicenses() const {
    return availableLicenses_;
}

int EBook::getTotalLicenses() const {
    return totalLicenses_;
}

bool EBook::borrowResource() {
    if (availableLicenses_ <= 0) {
        throw runtime_error("No digital license is currently available for this eBook.");
    }
    availableLicenses_--;
    setAvailability(availableLicenses_ > 0);
    return true;
}

void EBook::returnResource() {
    if (availableLicenses_ >= totalLicenses_) {
        throw runtime_error("All digital licenses are already available.");
    }
    availableLicenses_++;
    setAvailability(availableLicenses_ > 0);
}

int EBook::getAvailableCount() const {
    return availableLicenses_;
}

bool EBook::checkAvailability() const {
    return availableLicenses_ > 0;
}

// ---------------- DISPLAY (POLYMORPHISM SAFE) ----------------

void EBook::displayDetails() const {
    cout << "\n┌─────────────────────────────────┐\n";
    cout << "│    EBOOK DETAILS                  │\n";
    cout << "└─────────────────────────────────┘\n";
    cout << "ID: " << getResourceID() << endl;
    cout << "Title: " << getTitle() << endl;
    cout << "Author: " << getAuthor() << endl;
    cout << "Category: " << getCategory() << endl;

    cout << "Status: "
         << (checkAvailability() ? "Available" : "Unavailable")
         << endl;

    cout << "File Size (MB): " << fileSize_ << endl;
    cout << "Format: " << format_ << endl;
    cout << "Download Link: " << downloadLink_ << endl;
    cout << "Available Licenses: " << availableLicenses_ << " / " << totalLicenses_ << endl;
    cout << "Access Type: Online" << endl;
}

// ---------------- SERIALIZATION (FILE SAFE FIXED) ----------------

string EBook::serialize() const {
    return to_string(getResourceID()) + "|" +
           "EBook" + "|" +
           getTitle() + "|" +
           getAuthor() + "|" +
           getCategory() + "|" +
           (checkAvailability() ? "1" : "0") + "|" +
           to_string(fileSize_) + "|" +
           format_ + "|" +
           downloadLink_ + "|" +
           to_string(totalLicenses_) + "|" +
           getLocation();
}

EBook* EBook::deserialize(const string& line) {
    stringstream ss(line);
    string token;

    getline(ss, token, '|');
    const int id = stoi(token);

    getline(ss, token, '|');
    if (token != "EBook")
        throw ValidationException("Invalid resource type for ebook record.");

    string title;
    string author;
    string category;
    string format;
    string link;

    getline(ss, title, '|');
    getline(ss, author, '|');
    getline(ss, category, '|');
    getline(ss, token, '|');
    const bool isAvailable = (token == "1");
    getline(ss, token, '|');
    const float fileSize = stof(token);
    getline(ss, format, '|');
    getline(ss, link, '|');
    int licenseCount = 1;
    string location;
    if (getline(ss, token, '|')) {
        if (!token.empty())
            licenseCount = stoi(token);
        if (getline(ss, location, '|')) {
            // location loaded
        }
    }

    return new EBook(id, title, author, category, isAvailable, fileSize, format, link, licenseCount, location);
}
