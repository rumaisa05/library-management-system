#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>
#include "Validator.h"

enum class ResourceMode {
    Physical = 0,
    Digital = 1,
    Hybrid = 2
};

class Resource {
protected:
    int resourceID_;
    std::string title_;
    std::string author_;
    std::string category_;
    bool availabilityStatus_;
    ResourceMode mode_;
    std::string location_;

public:
    // Constructors
    Resource();
    Resource(int id, std::string t, std::string a, std::string c,
             bool status, ResourceMode mode = ResourceMode::Physical,
             std::string location = "");

    // Pure virtual (polymorphism)
    virtual std::string serialize() const = 0;
    virtual void displayDetails() const = 0;

    // Setters (validated)
    void setTitle(std::string t);
    void setAuthor(std::string a);
    void setCategory(std::string c);
    void setLocation(std::string location);

    // Getters
    int getResourceID() const;
    std::string getTitle() const;
    std::string getAuthor() const;
    std::string getCategory() const;
    ResourceMode getMode() const;
    std::string getLocation() const;
    bool isPhysical() const;
    bool isDigital() const;
    bool isHybrid() const;
    virtual int getAvailableCount() const;

    // Borrow system (safe)
    virtual bool borrowResource();
    virtual void returnResource();

    // Availability
    virtual bool checkAvailability() const;
    void setAvailability(bool status);

    // Operator overloading
    bool operator==(const Resource& other) const;

    virtual ~Resource() {}
};

#endif
