#include "../headers/User.h"
#include <utility>

User::User(std::string id, std::string pass, std::string n, std::string r)
    : userID(std::move(id)), password(std::move(pass)), name(std::move(n)), role(std::move(r)) {}

std::string User::getUserID() const { return userID; }
std::string User::getName() const { return name; }
std::string User::getRole() const { return role; }

bool User::verifyPassword(const std::string& pass) const {
    return password == pass;
}

