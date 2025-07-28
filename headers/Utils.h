#pragma once
#include <iostream>
#include <string>
#include <limits>
#include <vector>

namespace Utils {
    void clearScreen();
    void pause();
    int getValidatedInt(const std::string& prompt);
    int getValidatedInt(const std::string& prompt, int min, int max);
    std::string getNonEmptyString(const std::string& prompt);
    std::vector<std::string> split(const std::string& s, char delimiter);
    std::string toLower(std::string s);
}

