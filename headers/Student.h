
#pragma once
#include "User.h"
#include <string>

class Student : public User {
private:
    std::string className;
    std::string parentContact;

public:
    Student(std::string id, std::string pass, std::string n, std::string cName, std::string pContact);
    void displayMenu() override;
    std::string getClassName() const { return className; }

private:
    // Feature Functions
    void viewTimetable();
    void viewAssignments();
    void viewGrades();
    void viewAttendance();
    void viewCanteenBalance();
};