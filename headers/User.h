#pragma once
#include <string>
#include <vector>
#include "DataModels.h"

class User {
protected:
    std::string userID;
    std::string password;
    std::string name;
    std::string role;

public:
    User(std::string id, std::string pass, std::string n, std::string r);
    virtual ~User() = default;
    std::string getUserID() const;
    std::string getName() const;
    std::string getRole() const;
    bool verifyPassword(const std::string& pass) const;
    virtual void displayMenu() = 0;
};

