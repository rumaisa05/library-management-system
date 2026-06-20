#include "InputHelper.h"
#include "SYSTEMEXCEPTIONS.h"
#include "Validator.h"
#include <exception>
#include <iostream>

using namespace std;

namespace InputHelper {
bool askRetryOrExit()
{
    while (true) {
        cout << "Would you like to retry the input or exit this operation? (e/r): ";

        string choice;
        getline(cin >> ws, choice);

        if (choice == "e" || choice == "E")
            return false;

        if (choice == "r" || choice == "R")
            return true;

        cout << "Please enter 'r' to retry or 'e' to exit.\n";
    }
}

string readLine(const string& prompt)
{
    cout << prompt;

    string value;
    getline(cin >> ws, value);
    return value;
}

string readValidatedLine(const string& prompt, StringValidator validator)
{
    while (true) {
        const string value = readLine(prompt);

        try {
            validator(value);
            return value;
        }
        catch (const exception& e) {
            cout << "Error: " << e.what() << endl;

            if (!askRetryOrExit())
                throw OperationCancelledException();
        }
    }
}

int readValidatedInt(const string& prompt, IntValidator validator)
{
    while (true) {
        try {
            const int value = Validator::parseInt(readLine(prompt));
            validator(value);
            return value;
        }
        catch (const exception& e) {
            cout << "Error: " << e.what() << endl;

            if (!askRetryOrExit())
                throw OperationCancelledException();
        }
    }
}

int readMenuChoice(const string& prompt, int minValue, int maxValue)
{
    while (true) {
        try {
            const int value = Validator::parseInt(readLine(prompt));
            Validator::validateMenuChoice(value, minValue, maxValue);
            return value;
        }
        catch (const exception& e) {
            cout << "Error: " << e.what() << endl;

            if (!askRetryOrExit())
                throw OperationCancelledException();
        }
    }
}

string getCurrentDate()
{
    time_t now = time(nullptr);
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&now));
    return string(buf);
}

string getDueDate(int daysFromNow)
{
    time_t now = time(nullptr);
    now += (daysFromNow * 24 * 60 * 60); // Add daysFromNow days
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&now));
    return string(buf);
}

bool askConfirmation(const string& prompt)
{
    while (true) {
        string actualPrompt = prompt;
        if (prompt.find("(y/n)") == string::npos && prompt.find("(Y/N)") == string::npos) {
            actualPrompt += " (y/n): ";
        }
        cout << actualPrompt;

        string choice;
        getline(cin >> ws, choice);

        if (choice == "y" || choice == "Y")
            return true;

        if (choice == "n" || choice == "N")
            return false;

        cout << "Please enter 'y' for yes or 'n' for no.\n";
    }
}
}
