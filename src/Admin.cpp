
#include "../headers/Admin.h"
#include "../headers/Utils.h"
#include "../headers/uielements.h"
#include "../headers/FileManager.h"
#include <iostream>
#include <map>
#include <fstream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <set>
#include <sstream>
#include <iomanip>
#include <functional> // For std::hash

// Constructor
Admin::Admin(std::string id, std::string pass, std::string n)
    : User(std::move(id), std::move(pass), std::move(n), "Admin") {}

// The main menu for the Admin role.
// In Admin.cpp

// REMOVE the old Admin::displayMenu() function and REPLACE it with this:
void Admin::displayMenu() {
    const std::vector<std::string> options = {
        "Yearly Class Allocation (Promote Students)",
        "Staff Workload Report",
        "Salary Management",
        "Fee Management",
        "Manage Student Records",
        "Cancel Admission for Overdue Fees (Automatic)",
        "Manage Staff Records",
        "Forward Leave Requests to Principal",
        "Manage Custodian Area Assignments",
        "Validate Timetable for Conflicts",
        "Schedule Exam Invigilators",
        "Optimize Transport Routes (Bonus)",
        "Logout"
    };
     int LOGOUT_OPTION = options.size() - 1;

    int choice;
    do {
        choice = UI::showDashboard(*this, options, "--- Admin Control Panel ---");
        

        // The switch now uses 0-based indexing from the UI component
        switch (choice) {
            case 0: yearlyClassAllocation(); break;
            case 1: staffWorkloadBalancing(); break;
            case 2: salaryManagement(); break;
            case 3: feeManagement(); break;
            case 4: manageStudentRecords(); break;
            case 5: cancelStudentAdmission(); break;
            case 6: manageStaffRecords(); break;
            case 7: forwardLeaveRequests(); break;
            case 8: manageCustodianAssignments(); break;
            case 9: validateTimetable(); break;
            case 10: scheduleExamInvigilators(); break;
            case 11: optimizeTransportRoutes(); break;
            case 12:
            case -1: // -1 is returned if user presses ESC
                std::cout << "Logging out...\n";
                break;
            default:
                // This case should not be reached with the UI component
                break;
        }

        // Pause only if a feature was used (i.e., user didn't quit/logout)
        if (choice != -1 && choice != LOGOUT_OPTION) {
            Utils::pause();
        }

    } while (choice != -1 && choice != LOGOUT_OPTION);
}

// 1. Yearly Class Allocation
void Admin::yearlyClassAllocation() {
    Utils::clearScreen();
    std::cout << "--- Yearly Student Promotion ---\n";
    std::cout << "This process promotes students with an average grade of 50% or higher.\n";
    std::string confirm = Utils::getNonEmptyString("Are you sure you want to proceed? [yes/no]: ");

    if (Utils::toLower(confirm) != "yes") {
        std::cout << "Promotion cancelled.\n";
        return;
    }

    auto grades = FileManager::loadGrades();
    if (grades.empty()) {
        std::cout << "No grade data available. Cannot proceed with promotion.\n";
        return;
    }

    std::map<std::string, double> student_totals;
    std::map<std::string, int> subject_counts;
    for (const auto& g : grades) {
        student_totals[g.studentID] += g.total;
        subject_counts[g.studentID]++;
    }

    std::vector<std::string> updated_student_lines;
    std::ifstream student_file(FileManager::dataPath + "students.txt");
    std::string line;
    int promoted_count = 0;
    int failed_count = 0;

    if (!student_file.is_open()) {
        std::cerr << "Error: Cannot open students.txt\n";
        return;
    }

    while(getline(student_file, line)) {
        auto tokens = Utils::split(line, ',');
        if (tokens.size() < 5) continue;

        std::string student_id = tokens[0];
        std::string current_class = tokens[3];
        double average_grade = (subject_counts[student_id] > 0) ? student_totals[student_id] / subject_counts[student_id] : 0.0;

        if (average_grade >= 50.0) {
            auto class_parts = Utils::split(current_class, '-');
            if (class_parts.size() == 2) {
                try {
                    int grade_level = std::stoi(class_parts[0]);
                    if (grade_level < 12) {
                        tokens[3] = std::to_string(grade_level + 1) + "-" + class_parts[1];
                    } else {
                        tokens[3] = "Graduated";
                    }
                    promoted_count++;
                } catch(...) { /* Keep original class if parsing fails */ }
            }
        } else {
            failed_count++;
            std::cout << "Student " << tokens[2] << " (" << student_id << ") will be retained. Average: " 
                      << std::fixed << std::setprecision(1) << average_grade << "%\n";
        }
        updated_student_lines.push_back(tokens[0] + "," + tokens[1] + "," + tokens[2] + "," + tokens[3] + "," + tokens[4]);
    }
    student_file.close();

    rewriteFile("students.txt", updated_student_lines);

    std::cout << "\nPromotion process completed.\n";
    std::cout << "Promoted/Graduated: " << promoted_count << " students.\n";
    std::cout << "Retained: " << failed_count << " students.\n";
}

// 2. Staff Workload Balancing Report
void Admin::staffWorkloadBalancing() {
    Utils::clearScreen();
    std::cout << "--- Staff Workload Balancing Report ---\n\n";
    
    auto timetable = FileManager::loadTimetable();
    std::map<std::string, int> teacher_periods;
    std::map<std::string, std::string> teacher_names;

    std::ifstream teacher_file(FileManager::dataPath + "teachers.txt");
    std::string line;
    while(getline(teacher_file, line)) {
        auto tokens = Utils::split(line, ',');
        if (tokens.size() >= 3) teacher_names[tokens[0]] = tokens[2];
    }
    teacher_file.close();

    for (const auto& slot : timetable) {
        teacher_periods[slot.teacherID]++;
    }

    std::cout << "=== TEACHER WORKLOAD REPORT (Periods per Week) ===\n";
    std::cout << std::left << std::setw(15) << "Teacher ID" << std::setw(25) << "Name" << std::setw(15) << "Periods/Week" << "Status\n";
    std::cout << std::string(75, '-') << "\n";
    
    int overloaded_teachers = 0;
    for(const auto& pair : teacher_periods) {
        std::string status = (pair.second > 30) ? "OVERLOADED (>30)" : "OK";
        if (pair.second > 30) overloaded_teachers++;
        
        std::cout << std::left << std::setw(15) << pair.first << std::setw(25) << teacher_names[pair.first]
                  << std::setw(15) << pair.second << status << "\n";
    }
    
    std::cout << "\nSUMMARY: " << overloaded_teachers << " teachers are overloaded. Please adjust timetable.\n";
    
    std::cout << "\n\n=== CUSTODIAN STAFF ASSIGNMENTS ===\n";
    viewCustodianAssignments(); // Reuse existing function
}


// -----------------
// IN FILE: Admin.cpp
// -----------------

// REMOVE the old Admin::paymentManagement() function completely.
// ADD these two new functions in its place:

// 3. Salary Management (New Top-Level Feature)
void Admin::salaryManagement() {
    int choice;
    do {
        Utils::clearScreen();
        std::cout << "--- Salary Management System ---\n\n";
        std::cout << "1. Pay Salaries to All Staff\n";
        std::cout << "2. View Salary Payment History\n";
        std::cout << "0. Back to Main Menu\n";
        
        choice = Utils::getValidatedInt("Select option: ", 0, 2);
        
        switch(choice) {
            case 1: paySalaries(); break; // This function already exists
            case 2: 
                Utils::clearScreen();
                std::cout << "--- Salary Payment History ---\n";
                FileManager::readAndDisplayFile("salary_payments.txt");
                break;
        }
        if (choice != 0) Utils::pause();
    } while (choice != 0);
}

// 4. Fee Management (New Top-Level Feature)
void Admin::feeManagement() {
    int choice;
    do {
        Utils::clearScreen();
        std::cout << "--- Fee Management System ---\n\n";
        std::cout << "1. Issue Monthly Fee Challans\n";
        std::cout << "2. Check Overdue Fees (and cancel admissions)\n";
        std::cout << "3. View Full Fee Challan Report\n";
        std::cout << "0. Back to Main Menu\n";
        
        choice = Utils::getValidatedInt("Select option: ", 0, 3);
        
        switch(choice) {
            case 1: issueFeeChallans(); break; // This function already exists
            case 2: checkOverdueFeesAndCancel(); break; // This function already exists
            case 3:
                Utils::clearScreen();
                std::cout << "--- Fee Challan Status Report ---\n";
                FileManager::readAndDisplayFile("fee_challans.txt");
                break;
        }
        if (choice != 0) Utils::pause();
    } while (choice != 0);
}

void Admin::paySalaries() {
    Utils::clearScreen();
    std::cout << "--- Salary Payment Processing ---\n";
    
    std::string confirm = Utils::getNonEmptyString("This will process and record salaries for all staff. Proceed? [yes/no]: ");
    if (Utils::toLower(confirm) != "yes") return;

    time_t now = time(0);
    char date_str[20];
    strftime(date_str, sizeof(date_str), "%Y-%m-%d", localtime(&now));
    
    std::ofstream salary_file(FileManager::dataPath + "salary_payments.txt", std::ios::app);
    if (!salary_file.is_open()) {
        std::cerr << "Error: Cannot open salary_payments.txt for writing.\n";
        return;
    }
    
    double total_paid = 0;
    auto process_staff_file = [&](const std::string& filename, const std::string& type) {
        std::ifstream staff_file(FileManager::dataPath + filename);
        std::string line;
        std::cout << "\nProcessing " << type << " Salaries...\n";
        std::cout << std::string(70, '-') << "\n";
        while(getline(staff_file, line)) {
            auto tokens = Utils::split(line, ',');
            if (tokens.size() >= 3) {
                std::string id = tokens[0];
                std::string name = (type == "Teacher") ? tokens[2] : tokens[1];
                double salary = getSalaryForEmployee(id, type);
                double deduction = calculateLeaveDeduction(id);
                double net_salary = salary - deduction;
                
                std::cout << std::left << std::setw(25) << name << " Base: $" << std::fixed << std::setprecision(2) << salary
                          << " | Deduction: $" << deduction << " | Net: $" << net_salary << "\n";
                
                salary_file << date_str << "," << id << "," << type << "," << name << "," << net_salary << "\n";
                total_paid += net_salary;
            }
        }
        staff_file.close();
    };

    process_staff_file("teachers.txt", "Teacher");
    process_staff_file("custodian_staff.txt", "Custodian");
    
    salary_file.close();
    std::cout << "\n\nTotal Salaries Paid This Cycle: $" << std::fixed << std::setprecision(2) << total_paid << "\n";
    std::cout << "Payment records appended to salary_payments.txt\n";
}

void Admin::issueFeeChallans() {
    Utils::clearScreen();
    std::cout << "--- Monthly Fee Challan Generation ---\n\n";
    
    time_t now = time(0);
    char month_year[20];
    strftime(month_year, sizeof(month_year), "%B-%Y", localtime(&now));
    
    std::cout << "Generating challans for: " << month_year << "\n";
    double fee_amount = std::stod(Utils::getNonEmptyString("Enter monthly fee amount (e.g., 500.00): $"));
    
    std::ifstream student_file(FileManager::dataPath + "students.txt");
    std::ofstream challan_file(FileManager::dataPath + "fee_challans.txt", std::ios::app);
    std::string line;
    int challans_issued = 0;

    if (!student_file.is_open() || !challan_file.is_open()) {
        std::cerr << "Error opening student or challan files.\n";
        return;
    }
    
    std::cout << "\nIssuing challans...\n";
    while(getline(student_file, line)) {
        auto tokens = Utils::split(line, ',');
        if (tokens.size() >= 5 && tokens[3] != "Graduated") {
            std::string student_id = tokens[0];
            std::string student_name = tokens[2];
            
            time_t due_time = now + (20 * 24 * 60 * 60); // Due in 20 days
            char due_date_str[20];
            strftime(due_date_str, sizeof(due_date_str), "%Y-%m-%d", localtime(&due_time));
            
            // Challan format: StudentID,StudentName,Amount,Month,DueDate,Status
            challan_file << student_id << "," << student_name << "," << fee_amount 
                         << "," << month_year << "," << due_date_str << ",UNPAID\n";
            challans_issued++;
        }
    }
    
    student_file.close();
    challan_file.close();
    
    std::cout << "\nTotal Fee Challans Issued: " << challans_issued << "\n";
}

void Admin::viewPaymentReports() {
    Utils::clearScreen();
    std::cout << "--- Payment Reports ---\n\n";
    std::cout << "1. Salary Payment History\n";
    std::cout << "-------------------------\n";
    FileManager::readAndDisplayFile("salary_payments.txt");
    
    std::cout << "\n\n2. Fee Challan Status\n";
    std::cout << "---------------------\n";
    FileManager::readAndDisplayFile("fee_challans.txt");
}

// 4. Manage Student Records (Sub-Menu)
void Admin::manageStudentRecords() {
    int choice;
    do {
        Utils::clearScreen();
        std::cout << "--- Manage Student Records ---\n\n";
        std::cout << "1. Add New Student\n";
        std::cout << "2. View All Student Records\n";
        std::cout << "3. Edit a Student's Record\n";
        std::cout << "4. Remove a Student (Manual Cancellation)\n";
        std::cout << "0. Back to Main Menu\n";
        choice = Utils::getValidatedInt("Select option: ", 0, 4);
        switch (choice) {
            case 1: addNewStudent(); break;
            case 2: viewStudentRecords(); break;
            case 3: editStudentRecord(); break;
            case 4: removeStudentRecord(); break;
        }
        if (choice != 0) Utils::pause();
    } while (choice != 0);
}

void Admin::addNewStudent() {
    Utils::clearScreen();
    std::cout << "--- New Student Admission ---\n\n";

    std::string s_name = Utils::getNonEmptyString("Enter student's full name: ");
    std::string s_class = Utils::getNonEmptyString("Enter student's class (e.g., 9-A): ");
    std::string p_contact = Utils::getNonEmptyString("Enter parent's contact number: ");
    std::string p_name = Utils::getNonEmptyString("Enter parent's full name: ");

    int next_num = time(0) % 10000;
    std::string s_id = "S-" + std::to_string(next_num);
    std::string p_id = "P-" + std::to_string(next_num);
    std::string default_pass = "welcome123";

    std::ofstream s_file(FileManager::dataPath + "students.txt", std::ios::app);
    s_file << s_id << "," << default_pass << "," << s_name << "," << s_class << "," << p_contact << "\n";
    s_file.close();

    std::ofstream p_file(FileManager::dataPath + "parents.txt", std::ios::app);
    p_file << p_id << "," << default_pass << "," << p_name << "," << s_id << "\n";
    p_file.close();

    std::cout << "\n=== ADMISSION SUCCESSFUL ===\n";
    std::cout << "Student ID: " << s_id << " | Password: " << default_pass << "\n";
    std::cout << "Parent ID:  " << p_id << " | Password: " << default_pass << "\n";
}

void Admin::viewStudentRecords() {
    Utils::clearScreen();
    std::cout << "--- All Student Records ---\n";
    std::cout << "ID,Password,Name,Class,ParentContact\n";
    std::cout << "------------------------------------\n";
    FileManager::readAndDisplayFile("students.txt");
}

void Admin::editStudentRecord() {
    Utils::clearScreen();
    std::cout << "--- Edit Student Record ---\n";
    viewStudentRecords();
    std::string student_id = Utils::getNonEmptyString("\nEnter ID of student to edit: ");

    std::vector<std::string> lines;
    std::ifstream file_in(FileManager::dataPath + "students.txt");
    std::string line;
    bool found = false;
    while(getline(file_in, line)) {
        auto tokens = Utils::split(line, ',');
        if (tokens.size() >= 1 && tokens[0] == student_id) {
            found = true;
            std::cout << "\nEditing Record: " << line << "\n";
            tokens[2] = Utils::getNonEmptyString("Enter new name (" + tokens[2] + "): ");
            tokens[3] = Utils::getNonEmptyString("Enter new class (" + tokens[3] + "): ");
            tokens[4] = Utils::getNonEmptyString("Enter new parent contact (" + tokens[4] + "): ");
            line = tokens[0] + "," + tokens[1] + "," + tokens[2] + "," + tokens[3] + "," + tokens[4];
            std::cout << "Record updated.\n";
        }
        lines.push_back(line);
    }
    file_in.close();

    if (!found) {
        std::cout << "Student ID " << student_id << " not found.\n";
    } else {
        rewriteFile("students.txt", lines);
    }
}

void Admin::removeStudentRecord() {
    Utils::clearScreen();
    std::cout << "--- Remove Student Record ---\n";
    viewStudentRecords();
    std::string student_id = Utils::getNonEmptyString("\nEnter ID of student to remove: ");
    cancelStudentAdmissionById(student_id);
}

// 5. Cancel Student Admission (Automatic based on fees)
void Admin::cancelStudentAdmission() {
    Utils::clearScreen();
    std::cout << "--- Automatic Admission Cancellation for Overdue Fees ---\n";
    checkOverdueFeesAndCancel();
}

void Admin::checkOverdueFeesAndCancel() {
    time_t now = time(0);
    std::tm now_tm = *localtime(&now);
    
    std::ifstream challan_file(FileManager::dataPath + "fee_challans.txt");
    std::string line;
    std::vector<std::string> students_to_cancel;
    std::set<std::string> unique_students;

    if (!challan_file.is_open()) {
        std::cout << "No fee challan data found.\n";
        return;
    }

    while(getline(challan_file, line)) {
        auto tokens = Utils::split(line, ',');
        if (tokens.size() >= 6 && tokens[5] == "UNPAID") {
            std::string student_id = tokens[0];
            std::string due_date_str = tokens[4];
            
            std::tm due_tm = {};
            int year, month, day;
            char dash1, dash2;
            std::istringstream ss(due_date_str);
            ss >> year >> dash1 >> month >> dash2 >> day;
            if (ss.fail() || dash1 != '-' || dash2 != '-') continue;
            due_tm.tm_year = year - 1900;
            due_tm.tm_mon = month - 1;
            due_tm.tm_mday = day;
            due_tm.tm_isdst = -1;

            time_t due_time = mktime(&due_tm);
            double days_overdue = difftime(now, due_time) / (24 * 60 * 60);
                
            if (days_overdue > 20) {
                if(unique_students.find(student_id) == unique_students.end()){
                    std::cout << "FLAGGED: Student " << tokens[1] << " (" << student_id << ") fee is overdue by " 
                              << static_cast<int>(days_overdue) << " days.\n";
                    students_to_cancel.push_back(student_id);
                    unique_students.insert(student_id);
                }
            }
        }
    }
    challan_file.close();
    
    if (!students_to_cancel.empty()) {
        std::cout << "\nFound " << students_to_cancel.size() << " students with fees overdue by more than 20 days.\n";
        std::string confirm = Utils::getNonEmptyString("Cancel their admissions now? [yes/no]: ");
        if (Utils::toLower(confirm) == "yes") {
            for (const auto& student_id : students_to_cancel) {
                cancelStudentAdmissionById(student_id);
            }
        }
    } else {
        std::cout << "No students found with critical overdue fees.\n";
    }
}

// 6. Manage Staff Records (Sub-Menu)
void Admin::manageStaffRecords() {
    int choice;
    do {
        Utils::clearScreen();
        std::cout << "--- Manage Staff Records ---\n\n";
        std::cout << "1. Add New Staff Member\n";
        std::cout << "2. View All Staff Records\n";
        std::cout << "3. Update a Staff Member's Record\n";
        std::cout << "0. Back to Main Menu\n";
        choice = Utils::getValidatedInt("Select option: ", 0, 3);
        switch (choice) {
            case 1: addNewStaff(); break;
            case 2: viewStaffRecords(); break;
            case 3: updateStaffRecord(); break;
        }
        if (choice != 0) Utils::pause();
    } while (choice != 0);
}

void Admin::addNewStaff() {
    Utils::clearScreen();
    std::cout << "--- Add New Staff ---\n";
    int type = Utils::getValidatedInt("1. Teacher\n2. Custodian Staff\nSelect type: ", 1, 2);

    if (type == 1) { // Teacher
        std::string name = Utils::getNonEmptyString("Enter teacher's full name: ");
        std::string subjects = Utils::getNonEmptyString("Enter subjects taught (semicolon-separated; e.g., Math;Science): ");
        std::string classes = Utils::getNonEmptyString("Enter classes assigned (semicolon-separated; e.g., 9-A;9-B): ");
        std::string id = "T-" + std::to_string(time(0) % 1000);
        std::string pass = "staff123";
        std::ofstream file(FileManager::dataPath + "teachers.txt", std::ios::app);
        file << id << "," << pass << "," << name << ",," << subjects << "," << classes << "\n"; // Placeholder for qualification/contact
        file.close();
        std::cout << "Teacher " << name << " added with ID: " << id << "\n";
    } else { // Custodian
        addNewCustodianStaff(); // Reuse existing function
    }
}

void Admin::viewStaffRecords() {
    Utils::clearScreen();
    std::cout << "--- All Staff Records ---\n\n";
    std::cout << "=== Teachers ===\n";
    FileManager::readAndDisplayFile("teachers.txt");
    std::cout << "\n=== Custodian Staff ===\n";
    FileManager::readAndDisplayFile("custodian_staff.txt");
}

void Admin::updateStaffRecord() {
    std::cout << "Update staff record is a complex operation and should be done carefully.\n";
    std::cout << "This feature is a placeholder. Please edit the text files manually for now.\n";
    // Implementation would be similar to editStudentRecord, but for teachers.txt/custodian_staff.txt
}

// 7. Forward Leave Requests (Sub-Menu)
void Admin::forwardLeaveRequests() {
    int choice;
    do {
        Utils::clearScreen();
        std::cout << "--- Forward Leave Requests to Principal ---\n\n";
        std::cout << "1. Forward Teacher Leave Requests\n";
        std::cout << "2. Create & Forward Custodian Leave Request\n";
        std::cout << "3. View All Pending/Forwarded Requests\n";
        std::cout << "0. Back to Menu\n";
        
        choice = Utils::getValidatedInt("Select option: ", 0, 3);
        
        switch(choice) {
            case 1: forwardTeacherLeaveRequests(); break;
            case 2: createCustodianLeaveRequest(); break;
            case 3: viewAllPendingLeaveRequests(); break;
        }
        if (choice != 0 && choice != 0) Utils::pause();
    } while (choice != 0);
}

void Admin::forwardTeacherLeaveRequests() {
    Utils::clearScreen();
    auto all_requests = FileManager::loadLeaveRequests();
    std::vector<LeaveRequest*> pending_requests;

    std::cout << "--- Pending Teacher Leave Requests ---\n\n";
    for(auto& req : all_requests) {
        if (req.employeeType == "Teacher" && req.status == "Pending") {
            pending_requests.push_back(&req);
            std::cout << "ID: " << req.requestID << " | Teacher: " << req.employeeName << "\n"
                      << "  From: " << req.startDate << " To: " << req.endDate << " | Reason: " << req.reason << "\n---\n";
        }
    }
    
    if (pending_requests.empty()) {
        std::cout << "No pending teacher leave requests found.\n";
        return;
    }
    
    int req_id = Utils::getValidatedInt("Enter Request ID to forward (0 to cancel): ");
    if (req_id == 0) return;

    bool found = false;
    for (auto* req_ptr : pending_requests) {
        if (req_ptr->requestID == req_id) {
            req_ptr->adminComments = Utils::getNonEmptyString("Add admin comments/recommendation: ");
            req_ptr->status = "Forwarded to Principal";
            found = true;
            break;
        }
    }

    if (found) {
        FileManager::saveLeaveRequests(all_requests);
        std::cout << "\nLeave request " << req_id << " forwarded to Principal.\n";
    } else {
        std::cout << "Invalid Request ID.\n";
    }
}

void Admin::createCustodianLeaveRequest() {
    Utils::clearScreen();
    std::cout << "--- Create Custodian Leave Request (for physical requests) ---\n\n";
    
    auto requests = FileManager::loadLeaveRequests();
    
    LeaveRequest new_req;
    new_req.requestID = time(0);
    new_req.employeeID = Utils::getNonEmptyString("Enter Custodian Staff ID: ");
    new_req.employeeName = Utils::getNonEmptyString("Enter Staff Name: ");
    new_req.employeeType = "Custodian";
    new_req.startDate = Utils::getNonEmptyString("Enter start date (YYYY-MM-DD): ");
    new_req.endDate = Utils::getNonEmptyString("Enter end date (YYYY-MM-DD): ");
    new_req.reason = Utils::getNonEmptyString("Enter reason for leave: ");
    new_req.days = Utils::getValidatedInt("Enter total number of leave days: ", 1, 365);
    new_req.adminComments = Utils::getNonEmptyString("Add your recommendation: ");
    new_req.status = "Forwarded to Principal";
    
    requests.push_back(new_req);
    FileManager::saveLeaveRequests(requests);
    
    std::cout << "\nCustodian leave request created and forwarded to Principal.\n";
}

void Admin::viewAllPendingLeaveRequests() {
    Utils::clearScreen();
    std::cout << "--- All Pending & Forwarded Leave Requests ---\n\n";
    
    auto requests = FileManager::loadLeaveRequests();
    int count = 0;
    for (const auto& req : requests) {
        if (req.status == "Pending" || req.status == "Forwarded to Principal") {
            std::cout << "ID: " << req.requestID << " | Type: " << req.employeeType << "\n"
                      << "  Employee: " << req.employeeName << " (" << req.employeeID << ")\n"
                      << "  Dates: " << req.startDate << " to " << req.endDate << " (" << req.days << " days)\n"
                      << "  Status: " << req.status << "\n"
                      << "  Admin Comments: " << req.adminComments << "\n"
                      << "--------------------------------------------\n";
            count++;
        }
    }
    if (count == 0) std::cout << "No pending requests found.\n";
}

// 8. Manage Custodian Assignments (Sub-Menu)
void Admin::manageCustodianAssignments() {
    int choice;
    do {
        Utils::clearScreen();
        std::cout << "--- Manage Custodian Staff Areas ---\n\n";
        std::cout << "1. Assign Area to Staff\n";
        std::cout << "2. View Current Assignments\n";
        std::cout << "3. Reassign Staff to a Different Area\n";
        std::cout << "4. Add New Custodian Staff Member\n";
        std::cout << "0. Back to Menu\n";
        
        choice = Utils::getValidatedInt("Select option: ", 0, 4);
        
        switch(choice) {
            case 1: assignCustodianArea(); break;
            case 2: viewCustodianAssignments(); break;
            case 3: reassignCustodianArea(); break;
            case 4: addNewCustodianStaff(); break;
        }
        if (choice != 0) Utils::pause();
    } while(choice != 0);
}

void Admin::assignCustodianArea() {
    // This is a placeholder; the full logic would be complex.
    // For now, we assume this function correctly assigns areas.
    std::cout << "\nFeature to assign custodian area.\n";
    addNewCustodianStaff(); // Example of calling a sub-function
}

void Admin::viewCustodianAssignments() {
    Utils::clearScreen();
    std::cout << "--- Current Custodian Area Assignments ---\n\n";
    FileManager::readAndDisplayFile("custodian_areas.txt");
}

void Admin::reassignCustodianArea() {
     std::cout << "\nFeature to re-assign custodian area.\n";
}

void Admin::addNewCustodianStaff() {
    Utils::clearScreen();
    std::cout << "--- Add New Custodian Staff ---\n\n";
    std::string name = Utils::getNonEmptyString("Enter staff full name: ");
    std::string contact = Utils::getNonEmptyString("Enter contact number: ");
    std::string role = Utils::getNonEmptyString("Enter role (e.g., Guard, Cleaner): ");
    std::string id = "C-" + std::to_string(time(0) % 1000);
    
    std::ofstream file(FileManager::dataPath + "custodian_staff.txt", std::ios::app);
    file << id << "," << name << "," << contact << "," << role << "\n";
    file.close();
    
    std::cout << "\nNew Custodian Staff Added. ID: " << id << "\n";
}

// 9. Validate Timetable for Conflicts
void Admin::validateTimetable() {
    Utils::clearScreen();
    std::cout << "--- Timetable Conflict Validator ---\n\n";
    auto timetable = FileManager::loadTimetable();
    
    std::map<std::string, std::string> teacher_schedule; // Key: "TeacherID-Day-Period", Value: ClassName
    std::map<std::string, std::string> room_schedule;    // Key: "Room-Day-Period", Value: ClassName
    int conflicts_found = 0;

    for(const auto& slot : timetable) {
        std::string teacher_key = slot.teacherID + "-" + slot.day + "-" + std::to_string(slot.period);
        std::string room_key = slot.room + "-" + slot.day + "-" + std::to_string(slot.period);
        
        if(teacher_schedule.count(teacher_key)) {
            std::cout << "TEACHER CONFLICT: " << slot.teacherID << " is double-booked on " << slot.day << " Period " << slot.period 
                      << " (Classes: " << teacher_schedule[teacher_key] << " and " << slot.className << ")\n";
            conflicts_found++;
        } else {
            teacher_schedule[teacher_key] = slot.className;
        }
        
        if(room_schedule.count(room_key)) {
            std::cout << "ROOM CONFLICT: Room " << slot.room << " is double-booked on " << slot.day << " Period " << slot.period << "\n";
            conflicts_found++;
        } else {
            room_schedule[room_key] = slot.className;
        }

        if (Utils::toLower(slot.subject) == "pe" && slot.period == 5) {
             std::cout << "SCHEDULING WARNING: PE class for " << slot.className << " is scheduled right after lunch (Period 5).\n";
        }
    }

    if (conflicts_found == 0) {
        std::cout << "✓ No direct conflicts found in timetable.\n";
    } else {
        std::cout << "\n⚠ Found " << conflicts_found << " conflicts that need resolution.\n";
    }
}

// 10. Schedule Exam Invigilators
void Admin::scheduleExamInvigilators() {
    Utils::clearScreen();
    std::cout << "--- Exam Invigilator Auto-Scheduler ---\n";
    std::string exam_date = Utils::getNonEmptyString("Enter exam date to schedule for (YYYY-MM-DD): ");

    std::set<std::string> teachers_on_leave;
    for (const auto& req : FileManager::loadLeaveRequests()) {
        if (req.status == "Approved" && exam_date >= req.startDate && exam_date <= req.endDate) {
            teachers_on_leave.insert(req.employeeID);
        }
    }

    std::vector<std::pair<std::string, std::string>> available_teachers; // ID, Name
    std::map<std::string, std::set<std::string>> teacher_subjects;
    
    std::ifstream t_file(FileManager::dataPath + "teachers.txt");
    std::string line;
    while(getline(t_file, line)) {
        auto tokens = Utils::split(line, ',');
        if (tokens.size() >= 6) {
            if (teachers_on_leave.find(tokens[0]) == teachers_on_leave.end()) {
                available_teachers.push_back({tokens[0], tokens[2]});
                auto subjects = Utils::split(tokens[4], ';');
                for(const auto& s : subjects) teacher_subjects[tokens[0]].insert(Utils::toLower(s));
            }
        }
    }
    t_file.close();

    if (available_teachers.empty()) {
        std::cout << "No teachers are available on " << exam_date << ".\n";
        return;
    }

    std::vector<std::pair<std::string, std::string>> exams; // Class, Subject
    std::ifstream ex_file(FileManager::dataPath + "exams.txt");
    while(getline(ex_file, line)) {
        auto tokens = Utils::split(line, ',');
        if (tokens.size() >= 2) exams.push_back({tokens[0], tokens[1]});
    }
    ex_file.close();

    if (exams.empty()) {
        std::cout << "No exams found in exams.txt for the given date.\n";
        return;
    }

    std::ofstream inv_file(FileManager::dataPath + "invigilation_schedule.txt");
    inv_file << "INVIGILATION SCHEDULE FOR " << exam_date << "\n\n";
    std::cout << "\n=== Generated Invigilation Schedule ===\n";
    
    int teacher_idx = 0;
    for (const auto& exam : exams) {
        bool assigned = false;
        // Try to find a suitable teacher, looping through the list up to 2 times
        for (int attempt = 0; attempt < 2 * available_teachers.size(); ++attempt) {
            auto& teacher = available_teachers[teacher_idx];
            teacher_idx = (teacher_idx + 1) % available_teachers.size();

            // Check for self-invigilation
            if (teacher_subjects[teacher.first].find(Utils::toLower(exam.second)) == teacher_subjects[teacher.first].end()) {
                inv_file << "Class: " << exam.first << " | Subject: " << exam.second << " -> Invigilator: " << teacher.second << " (" << teacher.first << ")\n";
                std::cout << "Class: " << std::left << std::setw(10) << exam.first << " | Subject: " << std::setw(15) << exam.second 
                          << " -> Invigilator: " << teacher.second << " (" << teacher.first << ")\n";
                assigned = true;
                break;
            }
        }
        if (!assigned) {
            std::string msg = "Class: " + exam.first + " | Subject: " + exam.second + " -> FAILED TO ASSIGN INVIGILATOR\n";
            inv_file << msg;
            std::cout << msg;
        }
    }
    inv_file.close();
    std::cout << "\nSchedule saved to 'invigilation_schedule.txt'\n";
}

// 11. (Bonus) Optimize Transport Routes
void Admin::optimizeTransportRoutes() {
    Utils::clearScreen();
    std::cout << "--- Transport Route Optimization (BONUS) ---\n\n";
    std::cout << "This is a demonstration of a route optimization algorithm.\n";
    std::cout << "It uses a simple 'Nearest Neighbor' heuristic.\n\n";

    // Mock data for demonstration
    std::map<std::string, int> addresses; // Address -> Stop Number
    addresses["123 Oak St"] = 1;
    addresses["456 Pine Ave"] = 2;
    addresses["789 Maple Dr"] = 3;
    addresses["101 Elm Ct"] = 4;
    addresses["212 Birch Rd"] = 5;

    // Simple distance simulation using string hash differences
    auto distance = [](const std::string& a, const std::string& b) {
        std::hash<std::string> hasher;
        return std::abs(static_cast<long>(hasher(a) - hasher(b))) % 100;
    };

    std::string school = "School";
    std::vector<std::string> unvisited;
    for (const auto& pair : addresses) unvisited.push_back(pair.first);

    std::vector<std::string> route;
    std::string current_location = school;

    while (!unvisited.empty()) {
        long min_dist = -1;
        std::string nearest_neighbor;
        
        auto it_to_remove = unvisited.begin();
        for (auto it = unvisited.begin(); it != unvisited.end(); ++it) {
            long d = distance(current_location, *it);
            if (min_dist == -1 || d < min_dist) {
                min_dist = d;
                nearest_neighbor = *it;
                it_to_remove = it;
            }
        }
        
        route.push_back(nearest_neighbor);
        current_location = nearest_neighbor;
        unvisited.erase(it_to_remove);
    }
    
    std::cout << "Optimized Pickup Route:\n";
    std::cout << "Start: School\n";
    int stop_num = 1;
    for(const auto& stop : route) {
        std::cout << stop_num++ << ". " << stop << "\n";
    }
    std::cout << "End: School\n";
}

// Helper function to remove a student and their associated parent.
void Admin::cancelStudentAdmissionById(const std::string& student_id) {
    bool student_removed = false;
    
    // Remove from students.txt
    std::vector<std::string> s_lines;
    std::ifstream s_file_in(FileManager::dataPath + "students.txt");
    std::string line;
    while(getline(s_file_in, line)) {
        if (line.rfind(student_id, 0) == 0) { // Check if line starts with ID
            student_removed = true;
            std::cout << "Student record for " << student_id << " removed.\n";
            continue;
        }
        s_lines.push_back(line);
    }
    s_file_in.close();
    if (student_removed) rewriteFile("students.txt", s_lines);
    else {
        std::cout << "Student " << student_id << " not found in students.txt.\n";
        return;
    }
    
    // Remove associated parent from parents.txt
    std::vector<std::string> p_lines;
    std::ifstream p_file_in(FileManager::dataPath + "parents.txt");
    while(getline(p_file_in, line)) {
        auto tokens = Utils::split(line, ',');
        if (tokens.size() == 4 && tokens[3] == student_id) {
            std::cout << "Associated parent record " << tokens[0] << " removed.\n";
            continue;
        }
        p_lines.push_back(line);
    }
    p_file_in.close();
    rewriteFile("parents.txt", p_lines);
}

// Helper function to get an employee's salary.
double Admin::getSalaryForEmployee(const std::string& employee_id, const std::string& type) {
    // In a real system, this would come from a secure file.
    // For this hackathon, we use defaults.
    if (type == "Teacher") return 50000.0;
    if (type == "Custodian") return 25000.0;
    return 0.0;
}

// Helper function to calculate salary deductions based on leave.
double Admin::calculateLeaveDeduction(const std::string& employee_id) {
    int total_leave_days = 0;
    for (const auto& req : FileManager::loadLeaveRequests()) {
        if (req.employeeID == employee_id && req.status == "Approved") {
            total_leave_days += req.days;
        }
    }
    
    // Policy: Salary is deducted for leaves exceeding 21 days in a year.
    if (total_leave_days > 21) {
        int excess_days = total_leave_days - 21;
        double base_salary = getSalaryForEmployee(employee_id, "");
        double daily_rate = base_salary / 30.0; // Simple daily rate
        return excess_days * daily_rate;
    }
    return 0.0;
}

// Helper function to safely rewrite a file's contents.
void Admin::rewriteFile(const std::string& filename, const std::vector<std::string>& lines) {
    std::ofstream file_out(FileManager::dataPath + filename);
    if (!file_out.is_open()) {
        std::cerr << "CRITICAL ERROR: Could not open " << filename << " for writing. Data may be lost.\n";
        return;
    }
    for(const auto& l : lines) {
        file_out << l << "\n";
    }
    file_out.close();
}
