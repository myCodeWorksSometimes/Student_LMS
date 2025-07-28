#pragma once
#include <vector>
#include <memory>
#include "User.h"

class LMS {
private:
    std::vector<std::unique_ptr<User>> users;
    std::unique_ptr<User> currentUser;

    void loadAllData();
    void displayLoginScreen();
    bool attemptLogin(const std::string& id, const std::string& password);
public:
    LMS(); // bas ho gai, shukur, afaq left ;-;
    void run();
};

