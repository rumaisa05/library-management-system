#include "Resource.h"
#include <iostream>
#include <stdexcept>

using namespace std;

// ---------------- CONSTRUCTORS ----------------

Resource::Resource() {
    resourceID_ = 0;
    title_ = "";
    author_ = "";
    category_ = "";
    availabilityStatus_ = true;
    mode_ = ResourceMode::Physical;
    location_ = "";
}

Resource::Resource(int id, string t, string a, string c, bool status,
                   ResourceMode mode, string location) {
    try {
        Validator::validateID(id);
        Validator::validateString(t);
        Validator::validateName(a);
        Validator::validateCategory(c);

        resourceID_ = id;
        title_ = t;
        author_ = a;
        category_ = c;
        availabilityStatus_ = status;
        mode_ = mode;
        location_ = location;
    }
    catch (exception& e) {
        throw runtime_error("Resource creation failed: " + string(e.what()));
    }
}

// ---------------- SETTERS ----------------

void Resource::setTitle(string t) {
    Validator::validateString(t);
    title_ = t;
}

void Resource::setAuthor(string a) {
    Validator::validateName(a);
    author_ = a;
}

void Resource::setCategory(string c) {
    Validator::validateCategory(c);
    category_ = c;
}

// ---------------- GETTERS ----------------

int Resource::getResourceID() const {
    return resourceID_;
}

string Resource::getTitle() const {
    return title_;
}

string Resource::getAuthor() const {
    return author_;
}

string Resource::getCategory() const {
    return category_;
}

// ---------------- BORROW SYSTEM (IMPROVED) ----------------

bool Resource::borrowResource() {
    if (!availabilityStatus_) {
        throw runtime_error("Resource already borrowed");
    }

    availabilityStatus_ = false;
    return true;
}

void Resource::returnResource() {
    if (availabilityStatus_) {
        throw runtime_error("Resource was not borrowed");
    }

    availabilityStatus_ = true;
}

// ---------------- MODE & LOCATION ----------------

ResourceMode Resource::getMode() const {
    return mode_;
}

std::string Resource::getLocation() const {
    return location_;
}

void Resource::setLocation(std::string location) {
    location_ = location;
}

bool Resource::isPhysical() const {
    return mode_ == ResourceMode::Physical || mode_ == ResourceMode::Hybrid;
}

bool Resource::isDigital() const {
    return mode_ == ResourceMode::Digital || mode_ == ResourceMode::Hybrid;
}

bool Resource::isHybrid() const {
    return mode_ == ResourceMode::Hybrid;
}

int Resource::getAvailableCount() const {
    return availabilityStatus_ ? 1 : 0;
}

// ---------------- AVAILABILITY ----------------

bool Resource::checkAvailability() const {
    return availabilityStatus_;
}

void Resource::setAvailability(bool status) {
    availabilityStatus_ = status;
}

// ---------------- OPERATOR OVERLOAD ----------------

bool Resource::operator==(const Resource& other) const {
    return resourceID_ == other.resourceID_;
}
