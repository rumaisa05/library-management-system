#ifndef EBOOK_H
#define EBOOK_H

#include "Resource.h"
#include "Validator.h"
#include <string>

class EBook : public Resource {
private:
    float fileSize_;
    std::string format_;
    std::string downloadLink_;
    int availableLicenses_;
    int totalLicenses_;

public:
    // Constructors
    EBook();
    EBook(int id, std::string t, std::string a, std::string c, bool status,
          float fileSize, std::string format, std::string link,
          int licenseCount = 1, std::string location = "");

    // Setters
    void setFileSize(float size);
    void setFormat(std::string format);
    void setDownloadLink(std::string link);
    void setLicenseCount(int count);

    // Getters
    float getFileSize() const;
    std::string getFormat() const;
    std::string getDownloadLink() const;
    int getAvailableLicenses() const;
    int getTotalLicenses() const;

    // Override functions (IMPORTANT FIX)
    void displayDetails() const override;
    std::string serialize() const override;
    bool borrowResource() override;
    void returnResource() override;
    bool checkAvailability() const override;
    int getAvailableCount() const override;

    static EBook* deserialize(const std::string& line);

    ~EBook() {}
};

#endif
