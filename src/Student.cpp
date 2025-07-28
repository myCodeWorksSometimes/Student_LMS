
#include "../headers/Student.h"
#include "../headers/Utils.h"
#include "../headers/FileManager.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <map>

#include "../headers/uielements.h"


Student::Student(std::string id, std::string pass, std::string n, std::string cName, std::string pContact)
    : User(std::move(id), std::move(pass), std::move(n), "Student"), 
      className(std::move(cName)), parentContact(std::move(pContact)) {}

// In src/Student.cpp

void Student::displayMenu() {
    const std::vector<std::string> options = {
        "View My Timetable",
        "View Upcoming Assignments",
        "View My Grades / Report Card",
        "View My Attendance",
        "View Canteen Balance & History",
        "Logout"
    };
    const int LOGOUT_OPTION = options.size() - 1;

    int choice;
    do {
        // Display the dashboard and get the user's choice
        choice = UI::showDashboard(*this, options, "--- Student Portal ---");

        switch (choice) {
            case 0: viewTimetable(); break;
            case 1: viewAssignments(); break;
            case 2: viewGrades(); break;
            case 3: viewAttendance(); break;
            case 4: viewCanteenBalance(); break;
            case 5: // User selected "Logout"
            case -1:            // User pressed ESC
                std::cout << "Logging out...\n";
                break;
        }

        // Pause only if a feature was used (i.e., not quitting)
        if (choice != -1 && choice != LOGOUT_OPTION) {
            Utils::pause();
        }

    } while (choice != -1 && choice != LOGOUT_OPTION);
}


// 1. View My Timetable
void Student::viewTimetable() {
    Utils::clearScreen();
    std::cout << "--- Your Timetable for Class " << className << " ---\n\n";
    auto timetable = FileManager::loadTimetable();
    bool found = false;

    // A map to hold the schedule, keyed by day for organized printing
    std::map<std::string, std::vector<TimetableSlot>> daily_schedule;
    for (const auto& slot : timetable) {
        if (slot.className == this->className) {
            daily_schedule[slot.day].push_back(slot);
            found = true;
        }
    }

    if (!found) {
        std::cout << "No timetable has been uploaded for your class yet.\n";
        return;
    }

    // Print the organized schedule
    for (const auto& day_pair : daily_schedule) {
        std::cout << "=== " << day_pair.first << " ===\n";
        for (const auto& slot : day_pair.second) {
            std::cout << "  Period " << slot.period << ": " 
                      << std::left << std::setw(15) << slot.subject
                      << " | Teacher: " << std::setw(12) << slot.teacherID
                      << " | Room: " << slot.room << "\n";
        }
        std::cout << "\n";
    }
}

// 2. View Upcoming Assignments
void Student::viewAssignments() {
    // Helper function to calculate days difference
    auto getDaysLeft = [](const std::string& dueDateStr) -> int {
    time_t now = time(0);
    std::tm due_tm = {};
    
    int year, month, day;
    char dash1, dash2;
    std::istringstream ss(dueDateStr);
    ss >> year >> dash1 >> month >> dash2 >> day;

    if (ss.fail() || dash1 != '-' || dash2 != '-') {
        return -999; // Invalid date format
    }

    due_tm.tm_year = year - 1900;
    due_tm.tm_mon = month - 1;
    due_tm.tm_mday = day;
    due_tm.tm_hour = 23;
    due_tm.tm_min = 59;
    due_tm.tm_isdst = -1;

    time_t due_time = mktime(&due_tm);
    if (due_time == -1) return -999;

    return static_cast<int>(difftime(due_time, now) / (24 * 60 * 60));
};

    while (true) {
        Utils::clearScreen();
        std::cout << "--- My Assignment Dashboard ---\n\n";

        auto all_assignments = FileManager::loadAssignments();
        auto all_submissions = FileManager::loadSubmissions();

        // Create a map of my submissions for easy lookup
        std::map<int, Submission> my_submissions;
        for (const auto& sub : all_submissions) {
            if (sub.studentID == this->userID) {
                my_submissions[sub.assignmentID] = sub;
            }
        }
        
        bool has_assignments = false;
        std::vector<int> pending_assignment_ids; // To allow marking as done
        int display_idx = 1;

        std::cout << std::left << std::setw(4) << "#" << std::setw(15) << "Subject"
                  << std::setw(30) << "Details" << std::setw(15) << "Due Date"
                  << std::setw(15) << "Status" << "Time Left\n";
        std::cout << std::string(95, '-') << "\n";

        for (const auto& assign : all_assignments) {
            if (assign.className == this->className) {
                has_assignments = true;
                Submission sub = my_submissions[assign.assignmentID];
                int days_left = getDaysLeft(assign.dueDate);

                std::string time_status;
                if (sub.status != "Pending") {
                    time_status = "Submitted";
                } else if (days_left < 0) {
                    time_status = "OVERDUE";
                } else {
                    time_status = std::to_string(days_left) + " days";
                }

                std::cout << std::left << std::setw(4) << display_idx++
                          << std::setw(15) << assign.subject
                          << std::setw(30) << assign.details
                          << std::setw(15) << assign.dueDate
                          << std::setw(15) << sub.status
                          << time_status << "\n";
                
                if(sub.status == "Pending") {
                    pending_assignment_ids.push_back(assign.assignmentID);
                }
            }
        }
        std::cout << std::string(95, '-') << "\n";
        
        if (!has_assignments) {
            std::cout << "Woohoo! No assignments found for your class.\n";
            Utils::pause();
            return;
        }

        if (pending_assignment_ids.empty()) {
            std::cout << "Great job! All assignments have been marked as submitted.\n";
            Utils::pause();
            return;
        }

        int choice = Utils::getValidatedInt("\nEnter the # of a 'Pending' assignment to mark as done (0 to exit): ");
        if (choice == 0) return;

        if (choice > 0 && choice <= pending_assignment_ids.size()) {
            int assignment_to_mark = pending_assignment_ids[choice - 1];
            
            // Find and update the submission in the main list
            for (auto& sub : all_submissions) {
                if (sub.assignmentID == assignment_to_mark && sub.studentID == this->userID) {
                    sub.status = (getDaysLeft(all_assignments[0].dueDate) < 0) ? "Late" : "Submitted";
                    time_t now = time(0);
                    char date_buf[20];
                    strftime(date_buf, sizeof(date_buf), "%Y-%m-%d", localtime(&now));
                    sub.submissionDate = date_buf;
                    break;
                }
            }
            FileManager::saveSubmissions(all_submissions);
            std::cout << "Assignment marked as '" << ((getDaysLeft(all_assignments[0].dueDate) < 0) ? "Late" : "Submitted") << "'.\n";
            Utils::pause();
        } else {
            std::cout << "Invalid selection. Please choose a number corresponding to a 'Pending' assignment.\n";
            Utils::pause();
        }
    }
}

// 3. View My Grades / Report Card
void Student::viewGrades() {
    Utils::clearScreen();
    std::cout << "--- Your Grades & Report Card ---\n\n";
    auto grades = FileManager::loadGrades();
    bool found = false;
    double total_marks = 0;
    int subject_count = 0;

    std::cout << std::left << std::setw(20) << "Subject"
              << std::setw(10) << "Quiz(30%)"
              << std::setw(12) << "Midterm(40%)"
              << std::setw(10) << "Final(30%)"
              << std::setw(12) << "Total Score" << "\n";
    std::cout << std::string(75, '-') << "\n";

    for (const auto& grade : grades) {
        if (grade.studentID == this->userID) {
            std::cout << std::left << std::setw(20) << grade.subject
                      << std::setw(10) << grade.quiz
                      << std::setw(12) << grade.midterm
                      << std::setw(10) << grade.final
                      << std::fixed << std::setprecision(2) << std::setw(12) << grade.total << "\n";
            found = true;
            total_marks += grade.total;
            subject_count++;
        }
    }
    
    std::cout << std::string(75, '-') << "\n";
    if (!found) {
        std::cout << "No grades have been entered for you yet.\n";
    } else {
        double average = (subject_count > 0) ? total_marks / subject_count : 0.0;
        std::cout << std::left << std::setw(52) << "Overall Average:" 
                  << std::fixed << std::setprecision(2) << average << "%\n";
    }
}

// 4. View My Attendance
void Student::viewAttendance() {
    Utils::clearScreen();
    std::cout << "--- My Attendance History ---\n\n";
    auto attendance_records = FileManager::loadAttendance();
    
    std::map<std::string, int> summary; // Key: Status, Value: Count
    int total_records = 0;

    std::cout << std::left << std::setw(15) << "Date"
              << std::setw(15) << "Subject"
              << std::setw(10) << "Status" << "\n";
    std::cout << std::string(45, '-') << "\n";

    for (const auto& record : attendance_records) {
        if (record.studentID == this->userID) {
            std::cout << std::left << std::setw(15) << record.date
                      << std::setw(15) << record.subject
                      << std::setw(10) << record.status << "\n";
            summary[record.status]++;
            total_records++;
        }
    }

    std::cout << std::string(45, '-') << "\n";
    if (total_records == 0) {
        std::cout << "No attendance records found for you.\n";
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


// 5. View Canteen Balance
void Student::viewCanteenBalance() {
    Utils::clearScreen();
    std::cout << "--- Canteen Balance and History ---\n\n";
    std::ifstream file(FileManager::dataPath + "canteen_transactions.txt");
    if (!file.is_open()) {
        std::cout << "Canteen transaction data not available.\n";
        return;
    }

    std::string line;
    double balance = 0.0;
    std::cout << "Transaction History:\n";
    std::cout << "------------------------------------------\n";
    std::cout << std::left << std::setw(15) << "Date"
              << std::setw(15) << "Type"
              << "Amount\n";
    std::cout << "------------------------------------------\n";
    
    while(getline(file, line)) {
        auto tokens = Utils::split(line, ',');
        if (tokens.size() == 4 && tokens[0] == this->userID) {
            double amount = std::stod(tokens[2]);
            std::string type = tokens[1];
            std::string date = tokens[3];
            
            if (type == "TOPUP") {
                balance += amount;
                std::cout << std::left << std::setw(15) << date << std::setw(15) << "Top-up" 
                          << "+ $" << std::fixed << std::setprecision(2) << amount << "\n";
            } else if (type == "PURCHASE") {
                balance -= amount;
                std::cout << std::left << std::setw(15) << date << std::setw(15) << "Purchase"
                          << "- $" << std::fixed << std::setprecision(2) << amount << "\n";
            }
        }
    }
    file.close();

    std::cout << "------------------------------------------\n";
    std::cout << "Current Canteen Balance: $" << std::fixed << std::setprecision(2) << balance << "\n";
}
