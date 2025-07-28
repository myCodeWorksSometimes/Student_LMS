#include "../headers/Principal.h"
#include "../headers/Utils.h"
#include "../headers/FileManager.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include "../headers/uielements.h"

#include <map>

Principal::Principal(std::string id, std::string pass, std::string n)
    : User(std::move(id), std::move(pass), std::move(n), "Principal") {}

// In src/Principal.cpp

void Principal::displayMenu() {
    const std::vector<std::string> options = {
        "Review & Action Parent Requests",
        "Review & Approve Staff Leave",
        "Set & Approve Staff Salaries",
        "View School-Wide Reports (Dashboard)",
        "View Any Student/Staff Record",
        "Logout"
    };
    const int LOGOUT_OPTION = options.size() - 1;

    int choice;
    do {
        choice = UI::showDashboard(*this, options, "--- Principal's Office ---");

        switch (choice) {
            case 0: reviewParentRequests(); break;
            case 1: reviewStaffLeave(); break;
            case 2: manageSalaries(); break;
            case 3: viewSchoolWideReports(); break;
            case 4: lookupRecord(); break;
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
// 1. Review & Action Parent Requests
void Principal::reviewParentRequests() {
    Utils::clearScreen();
    std::cout << "--- Pending Parent Requests ---\n\n";
    
    std::vector<std::string> lines;
    std::vector<std::string> pending_requests;
    std::ifstream req_file(FileManager::dataPath + "parent_requests.txt");
    std::string line;
    
    while (getline(req_file, line)) {
        lines.push_back(line);
        auto tokens = Utils::split(line, ',');
        if (tokens.size() == 6 && tokens[5] == "Pending") {
            pending_requests.push_back(line);
        }
    }
    req_file.close();

    if (pending_requests.empty()) {
        std::cout << "No pending parent requests found.\n";
        return;
    }

    for(const auto& req_line : pending_requests) {
        auto tokens = Utils::split(req_line, ',');
        std::cout << "ID: " << tokens[0] << " | Child: " << tokens[2] << " | Type: " << tokens[3] << "\n"
                  << "  Details: " << tokens[4] << "\n"
                  << "--------------------------------------------------\n";
    }

    std::string reqID_str = Utils::getNonEmptyString("\nEnter Request ID to action (0 to cancel): ");
    if (reqID_str == "0") return;

    bool found_request = false;
    for (auto& l : lines) {
        auto tokens = Utils::split(l, ',');
        if (tokens.size() == 6 && tokens[0] == reqID_str && tokens[5] == "Pending") {
            found_request = true;
            int action = Utils::getValidatedInt("1. Approve\n2. Reject\nEnter choice: ", 1, 2);
            tokens[5] = (action == 1) ? "Approved" : "Rejected";
            l = tokens[0] + "," + tokens[1] + "," + tokens[2] + "," + tokens[3] + "," + tokens[4] + "," + tokens[5];
            std::cout << "\nRequest " << reqID_str << " has been updated to '" << tokens[5] << "'.\n";
            if (action == 1) {
                std::cout << "An order has been issued to the Admin to process this request.\n";
            }
            break;
        }
    }

    if (!found_request) {
        std::cout << "Request ID not found or is not pending.\n";
        return;
    }

    std::ofstream out_req_file(FileManager::dataPath + "parent_requests.txt");
    for(const auto& l_out : lines) {
        out_req_file << l_out << "\n";
    }
    out_req_file.close();
}

// 2. Review & Approve Staff Leave
void Principal::reviewStaffLeave() {
    Utils::clearScreen();
    std::cout << "--- Pending Staff Leave Requests ---\n\n";
    auto requests = FileManager::loadLeaveRequests();
    std::vector<LeaveRequest*> pending_requests;
    
    for (auto& req : requests) {
        if (req.status == "Forwarded to Principal") {
            pending_requests.push_back(&req);
        }
    }
    
    if (pending_requests.empty()) {
        std::cout << "No leave requests have been forwarded for approval.\n";
        return;
    }

    for (const auto* req_ptr : pending_requests) {
        std::cout << "ID: " << req_ptr->requestID << " | Type: " << req_ptr->employeeType << "\n"
                  << "  Employee: " << req_ptr->employeeName << " (" << req_ptr->employeeID << ")\n"
                  << "  Dates: " << req_ptr->startDate << " to " << req_ptr->endDate << " (" << req_ptr->days << " days)\n"
                  << "  Reason: " << req_ptr->reason << "\n"
                  << "  Admin Comments: " << req_ptr->adminComments << "\n"
                  << "--------------------------------------------------\n";
    }
    
    int reqID = Utils::getValidatedInt("\nEnter Request ID to action (0 to cancel): ");
    if(reqID == 0) return;

    auto it = std::find_if(requests.begin(), requests.end(), [reqID](const LeaveRequest& r){ 
        return r.requestID == reqID && r.status == "Forwarded to Principal";
    });
    
    if (it == requests.end()) {
        std::cout << "Request ID not found or is not pending your approval.\n";
        return;
    }
    
    int action = Utils::getValidatedInt("1. Approve\n2. Reject\nEnter choice: ", 1, 2);
    it->status = (action == 1) ? "Approved" : "Rejected";
    it->principalComments = Utils::getNonEmptyString("Add final comments: ");
    
    std::cout << "\nRequest " << reqID << " has been marked as '" << it->status << "'.\n";
    FileManager::saveLeaveRequests(requests);
}

// 3. Set & Approve Staff Salaries
void Principal::manageSalaries() {
    int choice;
    do {
        Utils::clearScreen();
        std::cout << "--- Salary Management ---\n\n";
        std::cout << "1. Set Salary for New Staff Member\n";
        std::cout << "2. Approve Salary Increment (Placeholder)\n";
        std::cout << "0. Back\n";
        choice = Utils::getValidatedInt("Your choice: ", 0, 2);
        if (choice == 1) setInitialSalary();
        if (choice == 2) approveSalaryIncrement();
    } while(choice != 0);
}

void Principal::setInitialSalary() {
    Utils::clearScreen();
    std::cout << "--- Set Initial Salary for New Staff ---\n\n";
    std::string emp_id = Utils::getNonEmptyString("Enter new staff member's ID (e.g., T-123 or C-456): ");
    std::string emp_type = Utils::getNonEmptyString("Enter staff type (Teacher/Custodian): ");
    double salary = std::stod(Utils::getNonEmptyString("Enter monthly salary amount: "));

    std::ofstream salary_file(FileManager::dataPath + "employee_salaries.txt", std::ios::app);
    if (!salary_file.is_open()) {
        std::cerr << "Error: Could not open employee_salaries.txt\n";
        return;
    }
    // Format: EmployeeID,Type,Salary
    salary_file << emp_id << "," << emp_type << "," << salary << "\n";
    salary_file.close();

    std::cout << "\nSalary of $" << std::fixed << std::setprecision(2) << salary << " has been set for " << emp_id << ".\n";
}

void Principal::approveSalaryIncrement() {
    std::cout << "\nThis is a placeholder for a future feature.\n";
    std::cout << "The Admin would submit an increment request, which would appear here for your approval.\n";
}


// 4. View School-Wide Reports
void Principal::viewSchoolWideReports() {
    Utils::clearScreen();
    std::cout << "--- School-Wide Report Dashboard ---\n\n";

    // Academic Report
    auto grades = FileManager::loadGrades();
    double total_score = 0;
    if (!grades.empty()) {
        for(const auto& g : grades) total_score += g.total;
        std::cout << ">> Academic Performance: " << std::fixed << std::setprecision(2) << (total_score / grades.size()) << "% (School-wide average score)\n";
    } else {
        std::cout << ">> Academic Performance: No grade data available.\n";
    }

    // Attendance Report
    auto attendance = FileManager::loadAttendance();
    int present = 0;
    if (!attendance.empty()) {
        for(const auto& a : attendance) if(a.status == "Present") present++;
        std::cout << ">> Student Attendance: " << std::fixed << std::setprecision(2) << (static_cast<double>(present) / attendance.size() * 100.0) << "% (Overall rate)\n";
    } else {
        std::cout << ">> Student Attendance: No attendance data available.\n";
    }

    // Staff Workload Report
    auto timetable = FileManager::loadTimetable();
    std::map<std::string, int> teacher_periods;
    for(const auto& slot : timetable) teacher_periods[slot.teacherID]++;
    int overloaded = 0;
    for(const auto& pair : teacher_periods) if(pair.second > 30) overloaded++;
    std::cout << ">> Staff Workload: " << overloaded << " out of " << teacher_periods.size() << " teachers are overloaded (>30 periods/week).\n";

    std::cout << "\n--- End of Report ---\n";
}

// 5. View Any Student/Staff Record
void Principal::lookupRecord() {
    Utils::clearScreen();
    std::cout << "--- Universal Record Lookup ---\n\n";
    std::string id = Utils::getNonEmptyString("Enter ID of student or staff member to view: ");
    
    char id_prefix = id.empty() ? ' ' : toupper(id[0]);
    std::string filename;
    
    if (id_prefix == 'S') filename = "students.txt";
    else if (id_prefix == 'T') filename = "teachers.txt";
    else if (id_prefix == 'C') filename = "custodian_staff.txt";
    else {
        std::cout << "Invalid ID format. Must start with 'S-', 'T-', or 'C-'.\n";
        return;
    }

    std::ifstream file(FileManager::dataPath + filename);
    std::string line;
    bool found = false;
    while(getline(file, line)) {
        if(line.rfind(id, 0) == 0) { // Check if line starts with the ID
            std::cout << "\nRecord Found in " << filename << ":\n";
            std::cout << "------------------------------------------\n";
            std::cout << line << "\n";
            std::cout << "------------------------------------------\n";
            found = true;
            break;
        }
    }
    file.close();

    if(!found) {
        std::cout << "No record found with ID '" << id << "' in " << filename << ".\n";
    }
}