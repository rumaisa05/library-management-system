#include "BookRating.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>

using namespace std;

// BookReview implementation
void BookReview::displayReview() const {
    cout << "\n[" << username_ << "] " << string(rating_, '*') << " (" << rating_ << "/5)\n";
    cout << "Date: " << reviewDate_ << "\n";
    cout << "Review: " << reviewText_ << "\n";
    cout << "Helpful: " << helpfulCount_ << " people found this helpful\n";
}

string BookReview::serialize() const {
    stringstream ss;
    ss << reviewID_ << "|" << userID_ << "|" << username_ << "|"
       << rating_ << "|" << reviewText_ << "|" << reviewDate_ << "|" << helpfulCount_;
    return ss.str();
}

BookReview* BookReview::deserialize(const string& line) {
    stringstream ss(line);
    string token;
    int id, userID, rating, helpful;
    string username, text, date;

    if (getline(ss, token, '|')) id = stoi(token);
    if (getline(ss, token, '|')) userID = stoi(token);
    if (getline(ss, token, '|')) username = token;
    if (getline(ss, token, '|')) rating = stoi(token);
    if (getline(ss, token, '|')) text = token;
    if (getline(ss, token, '|')) date = token;
    if (getline(ss, token, '|')) helpful = stoi(token);

    auto review = new BookReview(id, userID, username, rating, text, date);
    review->helpfulCount_ = helpful;
    return review;
}

// BookRating implementation
BookRating::BookRating(int resourceID)
    : resourceID_(resourceID), totalRatings_(0), averageRating_(0.0) {}

BookRating::~BookRating() {
    for (auto review : reviews_) {
        delete review;
    }
    reviews_.clear();
}

int BookRating::generateReviewID() const {
    int maxID = 0;
    for (const auto* review : reviews_) {
        if (review->getReviewID() > maxID)
            maxID = review->getReviewID();
    }
    return maxID + 1;
}

void BookRating::addRating(int userID, const string& username, int rating,
                          const string& reviewText) {
    if (rating < 1 || rating > 5) {
        throw runtime_error("Rating must be between 1 and 5 stars.");
    }

    // Check if user already reviewed this book
    for (const auto* review : reviews_) {
        if (review->getUserID() == userID) {
            throw runtime_error("You have already reviewed this book.");
        }
    }

    int reviewID = generateReviewID();
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    char dateStr[11];
    strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", timeinfo);

    auto newReview = new BookReview(reviewID, userID, username, rating, reviewText, dateStr);
    reviews_.push_back(newReview);
    totalRatings_++;
    recalculateAverageRating();
}

void BookRating::removeReview(int reviewID) {
    auto it = find_if(reviews_.begin(), reviews_.end(),
                     [reviewID](BookReview* r) { return r->getReviewID() == reviewID; });
    if (it != reviews_.end()) {
        delete *it;
        reviews_.erase(it);
        totalRatings_--;
        recalculateAverageRating();
    }
}

void BookRating::recalculateAverageRating() {
    if (reviews_.empty()) {
        averageRating_ = 0.0;
        return;
    }

    double sum = 0;
    for (const auto* review : reviews_) {
        sum += review->getRating();
    }
    averageRating_ = sum / reviews_.size();
}

void BookRating::displayRatingsSummary() const {
    cout << "\n┌─────────────────────────────────┐\n";
    cout << "│    BOOK RATINGS & REVIEWS         │\n";
    cout << "└─────────────────────────────────┘\n";
    if (totalRatings_ == 0) {
        cout << "No ratings yet.\n";
        return;
    }
    cout << "Average Rating: " << fixed << setprecision(1) << averageRating_ << "/5 ";
    cout << "(" << totalRatings_ << " reviews)\n";
    cout << "Stars: " << string(static_cast<int>(averageRating_), '*') << "\n";
}

void BookRating::displayAllReviews() const {
    if (reviews_.empty()) {
        cout << "No reviews available.\n";
        return;
    }
    
    cout << "\n┌─────────────────────────────────┐\n";
    cout << "│    ALL MEMBER REVIEWS (" << totalRatings_ << " total)            │\n";
    cout << "└─────────────────────────────────┘\n";
    for (size_t i = 0; i < reviews_.size(); i++) {
        cout << "\n[" << (i + 1) << "] ";
        reviews_[i]->displayReview();
    }
}

string BookRating::serialize() const {
    stringstream ss;
    ss << resourceID_ << ":" << totalRatings_ << ":" << fixed << setprecision(2) << averageRating_;
    for (const auto* review : reviews_) {
        ss << "|" << review->serialize();
    }
    return ss.str();
}

void BookRating::deserializeReviews(const string& line) {
    stringstream ss(line);
    string token;
    
    // Skip resource ID and counts
    getline(ss, token, ':');
    getline(ss, token, ':');
    
    // Parse reviews
    while (getline(ss, token, '|')) {
        if (!token.empty()) {
            auto review = BookReview::deserialize(token);
            if (review) reviews_.push_back(review);
        }
    }
    totalRatings_ = reviews_.size();
    recalculateAverageRating();
}
