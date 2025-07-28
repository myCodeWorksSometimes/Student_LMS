
#pragma once
#include "User.h"
#include <string>

class Parent : public User {
private:
    std::string childStudentID;

public:
    Parent(std::string id, std::string pass, std::string n, std::string childID);
    void displayMenu() override;

private:
    // Feature Functions
    void viewChildProgressReport();
    void viewChildAttendance();
    void viewFeeStatus();
    void requestChangeOfSection();
    void viewChildTimetable();
};
