#include "../headers/Utils.h"
#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>

void Utils::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void Utils::pause() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

int Utils::getValidatedInt(const std::string& prompt) {
    int value;
    std::cout << prompt;
    while (!(std::cin >> value)) {
        std::cout << "Invalid input. Please enter a number: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return value;
}

int Utils::getValidatedInt(const std::string& prompt, int min, int max) {
    int value;
    while (true) {
        value = getValidatedInt(prompt);
        if (value >= min && value <= max) {
            return value;
        }
        std::cout << "Input out of range. Please enter a value between " << min << " and " << max << ".\n";
    }
}

std::string Utils::getNonEmptyString(const std::string& prompt) {
    std::string input;
    do {
        std::cout << prompt;
        std::getline(std::cin, input);
        if (input.empty() && !std::cin.eof()) {
            std::cout << "Input cannot be empty. Please try again.\n";
        }
    } while (input.empty() && !std::cin.eof());
    return input;
}

std::vector<std::string> Utils::split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string Utils::toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return s;
}

