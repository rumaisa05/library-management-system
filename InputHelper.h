#ifndef INPUTHELPER_H
#define INPUTHELPER_H

#include <string>
#include <ctime>

namespace InputHelper {
using StringValidator = void (*)(const std::string&); // Function pointer , takes a const reference to a string and returns void (throws exception if invalid)
using IntValidator = void (*)(int);

bool askRetryOrExit();
std::string readLine(const std::string& prompt);
std::string readValidatedLine(const std::string& prompt, StringValidator validator);
int readValidatedInt(const std::string& prompt, IntValidator validator);
int readMenuChoice(const std::string& prompt, int minValue, int maxValue);

// Date helper functions
std::string getCurrentDate();
std::string getDueDate(int daysFromNow = 14);
bool askConfirmation(const std::string& prompt);
}

#endif
