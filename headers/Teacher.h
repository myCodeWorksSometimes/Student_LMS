
#pragma once
#include "User.h"
#include <vector>
#include <string>

class Teacher : public User {
private:
    std::vector<std::string> subjectsTaught;
    std::vector<std::string> classesAssigned;

public:
    Teacher(std::string id, std::string pass, std::string n, 
            const std::vector<std::string>& subjects, 
            const std::vector<std::string>& classes);
    void displayMenu() override;

private:
    void manageAssignments();
    // Feature Functions
    void manageGrades();
    void generateClassTermReports();
    void markStudentAttendance();
    void markPersonalAttendance();
    void applyForLeave();
    void viewMyTimetable();
    void viewMyWorkload();
    void viewDailyDuty();

    // Helper Functions
    bool isAuthorizedForClass(const std::string& className) const;
    bool isAuthorizedForSubject(const std::string& subjectName) const;
};