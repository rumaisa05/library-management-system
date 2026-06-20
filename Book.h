#ifndef BOOK_H
#define BOOK_H

#include "Resource.h"
#include "Validator.h"
#include <string>

class Book : public Resource {
private:
    std::string isbn_;
    std::string publisher_;
    int pages_;

public:
    // Constructors
    Book();
    Book(int id, std::string t, std::string a, std::string c,
         bool status, std::string isbn, std::string publisher, int pages,
         std::string location = "");

    // Override functions (IMPORTANT: const correctness)
    void displayDetails() const override;
    std::string serialize() const override;

    // Setters
    void setISBN(std::string isbn);
    void setPublisher(std::string publisher);
    void setPages(int pages);

    // Getters
    std::string getISBN() const;
    std::string getPublisher() const;
    int getPages() const;

    static Book* deserialize(const std::string& line);

    ~Book() {}
};

#endif
