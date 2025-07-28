
#include "../headers/Parent.h"
#include "../headers/Utils.h"
#include "../headers/FileManager.h"
#include "../headers/uielements.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <vector>
#include <map>

Parent::Parent(std::string id, std::string pass, std::string n, std::string childID)
    : User(std::move(id), std::move(pass), std::move(n), "Parent"), childStudentID(std::move(childID)) {}

// In src/Parent.cpp

void Parent::displayMenu() {
    const std::vector<std::string> options = {
        "View Child's Progress Report (Grades)",
        "View Child's Attendance",
        "View Fee Status & Payment History",
        "Request Change of Section",
        "View Child's Timetable",
        "Logout"
    };
    const int LOGOUT_OPTION = options.size() - 1;

    int choice;
    do {
        choice = UI::showDashboard(*this, options, "--- Parent Portal ---");

        switch (choice) {
            case 0: viewChildProgressReport(); break;
            case 1: viewChildAttendance(); break;
            case 2: viewFeeStatus(); break;
            case 3: requestChangeOfSection(); break;
            case 4: viewChildTimetable(); break;
            case 5:
            case -1:
                std::cout << "Logging out...\n";
                break;
        }
        
        if (choice != -1 && choice != LOGOUT_OPTION) {
            Utils::pause();
        }

    } while (choice != -1 && choice != LOGOUT_OPTION);
}
// 1. View Child's Progress Report
void Parent::viewChildProgressReport() {
    Utils::clearScreen();
    std::cout << "--- Progress Report for Student " << childStudentID << " ---\n\n";
    auto all_grades = FileManager::loadGrades();
    std::vector<Grade> child_grades;
    for (const auto& grade : all_grades) {
        if (grade.studentID == this->childStudentID) {
            child_grades.push_back(grade);
        }
    }

    if (child_grades.empty()) {
        std::cout << "No grades have been entered for your child yet.\n";
        return;
    }
    
    // Simple filter option
    std::string subject_filter = Utils::getNonEmptyString("Enter subject to filter by (or press Enter for all): ");

    std::cout << "\n" << std::left << std::setw(20) << "Subject"
              << std::setw(12) << "Quiz(30%)"
              << std::setw(14) << "Midterm(40%)"
              << std::setw(12) << "Final(30%)"
              << std::setw(12) << "Total Score" << "\n";
    std::cout << std::string(75, '-') << "\n";

    for (const auto& grade : child_grades) {
        if (subject_filter.empty() || Utils::toLower(grade.subject).find(Utils::toLower(subject_filter)) != std::string::npos) {
             std::cout << std::left << std::setw(20) << grade.subject
                      << std::setw(12) << grade.quiz
                      << std::setw(14) << grade.midterm
                      << std::setw(12) << grade.final
                      << std::fixed << std::setprecision(2) << std::setw(12) << grade.total << "%\n";
        }
    }
    std::cout << std::string(75, '-') << "\n";
}

// 2. View Child's Attendance
void Parent::viewChildAttendance() {
    Utils::clearScreen();
    std::cout << "--- Attendance History for Student " << childStudentID << " ---\n\n";
    auto attendance_records = FileManager::loadAttendance();
    
    std::map<std::string, int> summary;
    int total_records = 0;

    std::cout << std::left << std::setw(15) << "Date"
              << std::setw(15) << "Subject"
              << std::setw(10) << "Status" << "\n";
    std::cout << std::string(45, '-') << "\n";

    for (const auto& record : attendance_records) {
        if (record.studentID == this->childStudentID) {
            std::cout << std::left << std::setw(15) << record.date
                      << std::setw(15) << record.subject
                      << std::setw(10) << record.status << "\n";
            summary[record.status]++;
            total_records++;
        }
    }

    std::cout << std::string(45, '-') << "\n";
    if (total_records == 0) {
        std::cout << "No attendance records found for your child.\n";
    } else {
        std::cout << "\nSummary:\n";
        int present_count = summary.count("Present") ? summary["Present"] : 0;
        double percentage = (static_cast<double>(present_count) / total_records) * 100.0;
        std::cout << "  - Total Days Recorded: " << total_records << "\n";
        std::cout << "  - Present: " << present_count << "\n";
        std::cout << "  - Absent: " << (summary.count("Absent") ? summary["Absent"] : 0) << "\n";
        std::cout << "  - Attendance Percentage: " << std::fixed << std::setprecision(2) << percentage << "%\n";
    }
}

// 3. View Fee Status
void Parent::viewFeeStatus() {
    Utils::clearScreen();
    std::cout << "--- Fee Status and History for Student " << childStudentID << " ---\n\n";
    std::ifstream file(FileManager::dataPath + "fee_challans.txt");
    if(!file.is_open()) {
        std::cout << "Fee challan file not found. Please contact administration.\n";
        return;
    }
    
    std::string line;
    bool found_records = false;
    std::cout << std::left << std::setw(15) << "Month"
              << std::setw(12) << "Amount"
              << std::setw(15) << "Due Date"
              << "Status\n";
    std::cout << std::string(55, '-') << "\n";

    // Format: StudentID,StudentName,Amount,Month,DueDate,Status
    while(getline(file, line)) {
        auto tokens = Utils::split(line, ',');
        if(tokens.size() == 6 && tokens[0] == childStudentID) {
            std::cout << std::left << std::setw(15) << tokens[3] // Month
                      << std::setw(12) << ("$" + tokens[2]) // Amount
                      << std::setw(15) << tokens[4] // Due Date
                      << tokens[5] << "\n"; // Status
            found_records = true;
        }
    }
    
    std::cout << std::string(55, '-') << "\n";
    if(!found_records) {
        std::cout << "No fee records found for your child.\n";
    }
}

// 4. Request Change of Section
void Parent::requestChangeOfSection() {
    Utils::clearScreen();
    std::cout << "--- Submit Request to Principal ---\n";
    std::cout << "This form will generate a request to the Principal for a change of section.\n\n";
    
    std::string reason = Utils::getNonEmptyString("Please provide a brief reason for this request: ");

    std::ofstream req_file(FileManager::dataPath + "parent_requests.txt", std::ios::app);
    if (!req_file.is_open()) {
        std::cerr << "Error: Could not open request file for writing.\n";
        return;
    }

    int newID = time(0); // Simple unique ID
    std::string requestType = "Change of Section";

    // Format: ID,ParentID,ChildID,RequestType,Details,Status
    req_file << newID << "," << this->userID << "," << this->childStudentID << ","
             << requestType << "," << reason << ",Pending\n";
    
    req_file.close();

    std::cout << "\nYour request (ID " << newID << ") has been submitted successfully.\n";
    std::cout << "The Principal will review it shortly.\n";
}

// 5. View Child's Timetable
void Parent::viewChildTimetable() {
    Utils::clearScreen();
    // To get the class name, we must read the students file.
    std::string childs_class_name = "Unknown";
    std::ifstream student_file(FileManager::dataPath + "students.txt");
    std::string line;
    while(getline(student_file, line)) {
        auto tokens = Utils::split(line, ',');
        if (tokens.size() >= 4 && tokens[0] == childStudentID) {
            childs_class_name = tokens[3];
            break;
        }
    }
    student_file.close();

    std::cout << "--- Timetable for Class " << childs_class_name << " ---\n\n";
    if (childs_class_name == "Unknown") {
        std::cout << "Could not determine your child's class.\n";
        return;
    }
    
    auto timetable = FileManager::loadTimetable();
    bool found = false;
    std::map<std::string, std::vector<TimetableSlot>> daily_schedule;

    for (const auto& slot : timetable) {
        if (slot.className == childs_class_name) {
            daily_schedule[slot.day].push_back(slot);
            found = true;
        }
    }

    if (!found) {
        std::cout << "No timetable has been uploaded for your child's class yet.\n";
        return;
    }

    for (const auto& day_pair : daily_schedule) {
        std::cout << "=== " << day_pair.first << " ===\n";
        for (const auto& slot : day_pair.second) {
            std::cout << "  Period " << slot.period << ": " 
                      << std::left << std::setw(15) << slot.subject
                      << " | Teacher ID: " << std::setw(12) << slot.teacherID
                      << " | Room: " << slot.room << "\n";
        }
        std::cout << "\n";
    }
}