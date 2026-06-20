#ifndef BOOKRATING_H
#define BOOKRATING_H

#include <string>
#include <vector>
#include <ctime>

class BookReview {
private:
    int reviewID_;
    int userID_;
    std::string username_;
    int rating_;  // 1-5 stars
    std::string reviewText_;
    std::string reviewDate_;
    int helpfulCount_;

public:
    BookReview(int id, int userID, const std::string& username, int rating, 
               const std::string& text, const std::string& date)
        : reviewID_(id), userID_(userID), username_(username), rating_(rating),
          reviewText_(text), reviewDate_(date), helpfulCount_(0) {}

    // Getters
    int getReviewID() const { return reviewID_; }
    int getUserID() const { return userID_; }
    std::string getUsername() const { return username_; }
    int getRating() const { return rating_; }
    std::string getReviewText() const { return reviewText_; }
    std::string getReviewDate() const { return reviewDate_; }
    int getHelpfulCount() const { return helpfulCount_; }

    // Methods
    void markHelpful() { helpfulCount_++; }
    void displayReview() const;
    std::string serialize() const;
    static BookReview* deserialize(const std::string& line);
};

class BookRating {
private:
    int resourceID_;
    int totalRatings_;
    double averageRating_;
    std::vector<BookReview*> reviews_;

public:
    BookRating(int resourceID);
    ~BookRating();

    // Rating management
    void addRating(int userID, const std::string& username, int rating, 
                   const std::string& reviewText);
    void removeReview(int reviewID);
    double getAverageRating() const { return averageRating_; }
    int getTotalRatings() const { return totalRatings_; }
    const std::vector<BookReview*>& getReviews() const { return reviews_; }

    // Display
    void displayRatingsSummary() const;
    void displayAllReviews() const;

    // Serialization
    std::string serialize() const;
    void deserializeReviews(const std::string& line);

private:
    void recalculateAverageRating();
    int generateReviewID() const;
};

#endif
