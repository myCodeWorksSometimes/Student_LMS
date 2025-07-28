#pragma once
#include "User.h"

class Principal : public User {
public:
    Principal(std::string id, std::string pass, std::string n);
    void displayMenu() override;

private:
    // Feature Functions
    void reviewParentRequests();
    void reviewStaffLeave();
    void manageSalaries();
    void viewSchoolWideReports();
    void lookupRecord();

    // Helper for manageSalaries
    void setInitialSalary();
    void approveSalaryIncrement();
};
