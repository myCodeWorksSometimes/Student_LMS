
#include "../headers/Teacher.h"
#include "../headers/Utils.h"
#include "../headers/FileManager.h"
#include "../headers/uielements.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <map> // needed for student selection, i hate this library

Teacher::Teacher(std::string id, std::string pass, std::string n, 
                 const std::vector<std::string>& subjects, 
                 const std::vector<std::string>& classes)
    : User(std::move(id), std::move(pass), std::move(n), "Teacher"),
      subjectsTaught(subjects), classesAssigned(classes) {}

// In src/Teacher.cpp

void Teacher::displayMenu() {
    const std::vector<std::string> options = {
        "Manage Student Grades",
        "Generate Term Reports for a Class",
        "Mark Student Attendance",
        "Mark My Personal Attendance",
        "Apply for Leave",
        "View My Timetable",
        "View My Workload",
        "Manage Assignments",
        "View Daily Duty Roster",
        "Logout"
    };
    const int LOGOUT_OPTION = options.size() - 1;

    int choice;
    do {
        choice = UI::showDashboard(*this, options, "--- Teacher Dashboard ---");

        switch (choice) {
            case 0: manageGrades(); break;
            case 1: generateClassTermReports(); break;
            case 2: markStudentAttendance(); break;
            case 3: markPersonalAttendance(); break;
            case 4: applyForLeave(); break;
            case 5: viewMyTimetable(); break;
            case 6: viewMyWorkload(); break;
            case 7: manageAssignments(); break;
            case 8: viewDailyDuty(); break;
            case 9:
            case -1:
                std::cout << "Logging out...\n";
                break;
        }

        if (choice != -1 && choice != LOGOUT_OPTION) {
            Utils::pause();
        }

    } while (choice != -1 && choice != LOGOUT_OPTION);
}
// 1. Manage Grades (Refactored for descriptiveness)
void Teacher::manageGrades() {
    Utils::clearScreen();
    std::cout << "--- Grade Entry System ---\n\n";

    // Step 1: Select a Class from a list
    if (classesAssigned.empty()) {
        std::cout << "You are not assigned to any classes.\n";
        return;
    }
    std::cout << "Select a class to manage grades for:\n";
    for (size_t i = 0; i < classesAssigned.size(); ++i) {
        std::cout << i + 1 << ". " << classesAssigned[i] << "\n";
    }
    int class_choice = Utils::getValidatedInt("Your choice (0 to cancel): ", 0, classesAssigned.size());
    if (class_choice == 0) return;
    std::string selected_class_name = classesAssigned[class_choice - 1];

    // Step 2: Select a Subject from a list
    if (subjectsTaught.empty()) {
        std::cout << "You do not have any subjects assigned.\n";
        return;
    }
    std::cout << "\nSelect a subject:\n";
    for (size_t i = 0; i < subjectsTaught.size(); ++i) {
        std::cout << i + 1 << ". " << subjectsTaught[i] << "\n";
    }
    int subject_choice = Utils::getValidatedInt("Your choice (0 to cancel): ", 0, subjectsTaught.size());
    if (subject_choice == 0) return;
    std::string selected_subject_name = subjectsTaught[subject_choice - 1];

    // Step 3: Select a Student from the chosen class
    std::vector<std::pair<std::string, std::string>> class_students; // ID, Name
    std::ifstream student_file(FileManager::dataPath + "students.txt");
    std::string line;
    while (getline(student_file, line)) {
        auto tokens = Utils::split(line, ',');
        if (tokens.size() == 5 && tokens[3] == selected_class_name) {
            class_students.push_back({tokens[0], tokens[2]});
        }
    }
    student_file.close();

    if (class_students.empty()) {
        std::cout << "No students found in class " << selected_class_name << ".\n";
        return;
    }
    std::cout << "\nSelect a student from class " << selected_class_name << ":\n";
    for (size_t i = 0; i < class_students.size(); ++i) {
        std::cout << i + 1 << ". " << class_students[i].second << " (" << class_students[i].first << ")\n";
    }
    int student_choice = Utils::getValidatedInt("Your choice (0 to cancel): ", 0, class_students.size());
    if (student_choice == 0) return;
    std::string selected_student_id = class_students[student_choice - 1].first;

    // Step 4: Enter the grades (existing logic)
    auto allGrades = FileManager::loadGrades();
    auto it = std::find_if(allGrades.begin(), allGrades.end(), [&](const Grade& g) {
        return g.studentID == selected_student_id && Utils::toLower(g.subject) == Utils::toLower(selected_subject_name);
    });

    Grade grade_record;
    if (it != allGrades.end()) {
        grade_record = *it;
        std::cout << "\nExisting grades found for this student. You can update them.\n";
    } else {
        grade_record.studentID = selected_student_id;
        grade_record.subject = selected_subject_name;
        std::cout << "\nNo existing grade record found. Creating a new one.\n";
    }

    grade_record.quiz = Utils::getValidatedInt("Enter Quiz score (0-100): ", 0, 100);
    grade_record.midterm = Utils::getValidatedInt("Enter Midterm score (0-100): ", 0, 100);
    grade_record.final = Utils::getValidatedInt("Enter Final score (0-100): ", 0, 100);
    grade_record.total = (grade_record.quiz * 0.3) + (grade_record.midterm * 0.4) + (grade_record.final * 0.3);

    if (it != allGrades.end()) *it = grade_record;
    else allGrades.push_back(grade_record);

    FileManager::saveGrades(allGrades);
    std::cout << "\nGrades saved successfully for " << selected_student_id << " in " << selected_subject_name << ".\n";
    std::cout << "Calculated Total: " << std::fixed << std::setprecision(2) << grade_record.total << "%\n";
}

// 2. Generate Term Reports (Refactored for descriptiveness)
void Teacher::generateClassTermReports() {
    Utils::clearScreen();
    std::cout << "--- Generate Term Reports for a Class ---\n\n";
    
    // Step 1: Select a Class from a list
    if (classesAssigned.empty()) {
        std::cout << "You are not assigned to any classes.\n";
        return;
    }
    std::cout << "Select a class to generate reports for:\n";
    for (size_t i = 0; i < classesAssigned.size(); ++i) {
        std::cout << i + 1 << ". " << classesAssigned[i] << "\n";
    }
    int class_choice = Utils::getValidatedInt("Your choice (0 to cancel): ", 0, classesAssigned.size());
    if (class_choice == 0) return;
    std::string selected_class_name = classesAssigned[class_choice - 1];

    // Step 2: Generate reports (existing logic)
    auto all_grades = FileManager::loadGrades();
    std::ifstream student_file(FileManager::dataPath + "students.txt");
    std::string line;
    int reports_generated = 0;

    std::cout << "\nGenerating reports for class " << selected_class_name << "...\n";

    while(getline(student_file, line)) {
        auto tokens = Utils::split(line, ',');
        if(tokens.size() == 5 && tokens[3] == selected_class_name) {
            std::string student_id = tokens[0];
            std::string student_name = tokens[2];
            
            std::string report = "--- TERM REPORT FOR " + student_name + " (" + student_id + ") ---\n";
            report += "Class: " + selected_class_name + "\n";
            report += "--------------------------------------------------\n";
            report += "Subject             | Quiz | Midterm | Final | Total\n";
            report += "--------------------------------------------------\n";
            
            bool found_grades = false;
            for(const auto& grade : all_grades) {
                if (grade.studentID == student_id) {
                    found_grades = true;
                    std::ostringstream line_ss;
                    line_ss << std::left << std::setw(20) << grade.subject << "| "
                            << std::setw(5) << grade.quiz << "| "
                            << std::setw(8) << grade.midterm << "| "
                            << std::setw(6) << grade.final << "| "
                            << std::fixed << std::setprecision(2) << grade.total << "%\n";
                    report += line_ss.str();
                }
            }
            if(found_grades) {
                report += "--------------------------------------------------\n\n";
                FileManager::appendToReport(report);
                reports_generated++;
            }
        }
    }
    student_file.close();

    if (reports_generated > 0) {
        std::cout << reports_generated << " term reports have been generated and stored in termReports.txt\n";
    } else {
        std::cout << "No grades found for students in class " << selected_class_name << ". No reports generated.\n";
    }
}


// 3. Mark Student Attendance (Refactored for descriptiveness)
void Teacher::markStudentAttendance() {
    Utils::clearScreen();
    std::cout << "--- Mark Student Attendance ---\n\n";

    // Step 1: Select a Class from a list
    if (classesAssigned.empty()) {
        std::cout << "You are not assigned to any classes.\n";
        return;
    }
    std::cout << "Select a class to mark attendance for:\n";
    for (size_t i = 0; i < classesAssigned.size(); ++i) {
        std::cout << i + 1 << ". " << classesAssigned[i] << "\n";
    }
    int class_choice = Utils::getValidatedInt("Your choice (0 to cancel): ", 0, classesAssigned.size());
    if (class_choice == 0) return;
    std::string selected_class_name = classesAssigned[class_choice - 1];
    
    // Step 2: Get date and subject
    std::string date = Utils::getNonEmptyString("\nEnter date (YYYY-MM-DD): ");
    
    if (subjectsTaught.empty()) {
        std::cout << "You do not have any subjects assigned.\n";
        return;
    }
    std::cout << "\nSelect the subject for this period:\n";
    for (size_t i = 0; i < subjectsTaught.size(); ++i) {
        std::cout << i + 1 << ". " << subjectsTaught[i] << "\n";
    }
    int subject_choice = Utils::getValidatedInt("Your choice (0 to cancel): ", 0, subjectsTaught.size());
    if (subject_choice == 0) return;
    std::string selected_subject_name = subjectsTaught[subject_choice - 1];
    
    // Step 3: Mark attendance (existing logic)
    std::ifstream student_file(FileManager::dataPath + "students.txt");
    std::string line;
    std::vector<AttendanceRecord> new_records;
    
    std::cout << "\nPlease mark attendance for students in " << selected_class_name << "...\n";
    while(getline(student_file, line)) {
        auto tokens = Utils::split(line, ',');
        if(tokens.size() == 5 && tokens[3] == selected_class_name) {
            std::string student_id = tokens[0];
            std::string student_name = tokens[2];
            std::string status_input;
            do {
                status_input = Utils::getNonEmptyString("Status for " + student_name + " (" + student_id + ") [P/A]: ");
                status_input = Utils::toLower(status_input);
            } while (status_input != "p" && status_input != "a");
            
            std::string status = (status_input == "p") ? "Present" : "Absent";
            new_records.push_back({student_id, date, selected_class_name, status, this->userID, selected_subject_name});
        }
    }
    student_file.close();

    if (new_records.empty()) {
        std::cout << "No students found for class " << selected_class_name << ".\n";
    } else {
        FileManager::saveAttendance(new_records);
        std::cout << "\nAttendance for " << selected_class_name << " on " << date << " has been saved.\n";
    }
}

// 4. Mark Personal Attendance
void Teacher::markPersonalAttendance() {
    Utils::clearScreen();
    std::cout << "--- Mark My Personal Attendance ---\n";

    time_t now = time(0);
    char date_buf[20];
    strftime(date_buf, sizeof(date_buf), "%Y-%m-%d", localtime(&now));
    std::string today_date(date_buf);

    std::ifstream attendance_file_in(FileManager::dataPath + "teacher_attendance.txt");
    std::string line;
    bool already_marked = false;
    while(getline(attendance_file_in, line)) {
        if (line.find(this->userID) != std::string::npos && line.find(today_date) != std::string::npos) {
            already_marked = true;
            break;
        }
    }
    attendance_file_in.close();

    if(already_marked) {
        std::cout << "You have already marked your attendance for today (" << today_date << ").\n";
    } else {
        std::cout << "Marking attendance for " << today_date << ".\n";
        std::ofstream attendance_file_out(FileManager::dataPath + "teacher_attendance.txt", std::ios::app);
        attendance_file_out << this->userID << "," << today_date << ",Present\n";
        attendance_file_out.close();
        std::cout << "Your attendance has been successfully marked as 'Present'.\n";
    }
}


// 5. Apply for Leave
void Teacher::applyForLeave() {
    Utils::clearScreen();
    std::cout << "--- Apply for Leave ---\n";
    
    std::string startDate = Utils::getNonEmptyString("Enter start date (YYYY-MM-DD): ");
    std::string endDate = Utils::getNonEmptyString("Enter end date (YYYY-MM-DD): ");
    std::string reason = Utils::getNonEmptyString("Enter reason for leave: ");
    int days = Utils::getValidatedInt("Enter total number of leave days: ", 1, 365);

    auto requests = FileManager::loadLeaveRequests();
    
    int newID = time(0);
    
    LeaveRequest newReq;
    newReq.requestID = newID;
    newReq.employeeID = this->userID;
    newReq.employeeName = this->name;
    newReq.employeeType = "Teacher";
    newReq.startDate = startDate;
    newReq.endDate = endDate;
    newReq.reason = reason;
    newReq.days = days;
    newReq.status = "Pending";
    
    requests.push_back(newReq);
    FileManager::saveLeaveRequests(requests);
    
    std::cout << "\nLeave request submitted successfully. It is now pending review by the Admin.\n";
}

// 6. View My Timetable
void Teacher::viewMyTimetable() {
    Utils::clearScreen();
    std::cout << "--- My Weekly Timetable ---\n\n";
    auto timetable = FileManager::loadTimetable();
    
    std::map<std::string, std::vector<TimetableSlot>> daily_schedule;
    for (const auto& slot : timetable) {
        if (slot.teacherID == this->userID) {
            daily_schedule[slot.day].push_back(slot);
        }
    }

    if (daily_schedule.empty()) {
        std::cout << "No classes found on your timetable.\n";
        return;
    }

    // You can add logic here to sort days if needed (e.g., Monday, Tuesday...)
    for (const auto& day_pair : daily_schedule) {
        std::cout << "=== " << day_pair.first << " ===\n";
        for (const auto& slot : day_pair.second) {
             std::cout << "  Period " << slot.period << ": " 
                       << std::left << std::setw(15) << slot.subject
                       << " | Class: " << std::setw(8) << slot.className
                       << " | Room: " << slot.room << "\n";
        }
        std::cout << "\n";
    }
}

// 7. View My Workload
void Teacher::viewMyWorkload() {
    Utils::clearScreen();
    std::cout << "--- My Weekly Workload ---\n\n";
    auto timetable = FileManager::loadTimetable();
    int period_count = 0;
    for (const auto& slot : timetable) {
        if (slot.teacherID == this->userID) {
            period_count++;
        }
    }
    
    std::cout << "You are assigned " << period_count << " periods per week.\n";
    if (period_count > 30) {
        std::cout << "WARNING: Your workload exceeds the recommended 30 periods per week.\n";
    } else {
        std::cout << "Your workload is within the recommended limit.\n";
    }
}

// 8. View Daily Duty Roster
void Teacher::viewDailyDuty() {
    Utils::clearScreen();
    std::cout << "--- My Assigned Duties ---\n\n";
    std::string date = Utils::getNonEmptyString("Enter date to check (YYYY-MM-DD) or press Enter for today: ");
    if (date.empty()) {
        time_t now = time(0);
        char buf[20];
        strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&now));
        date = buf;
    }
    
    std::ifstream file(FileManager::dataPath + "duty_roster.txt");
    if (!file.is_open()){
        std::cout << "Duty roster file (duty_roster.txt) not found.\n";
        return;
    }
    std::string line;
    bool found_duty = false;
    while(getline(file, line)) {
        auto tokens = Utils::split(line, ',');
        if (tokens.size() == 3 && tokens[0] == date && tokens[2] == this->userID) {
            std::cout << "On " << date << ", you are assigned to: " << tokens[1] << "\n";
            found_duty = true;
        }
    }
    file.close();
    
    if (!found_duty) {
        std::cout << "No specific duties found for you on " << date << ".\n";
    }
}

// Helper function to check if the teacher is assigned to a specific class.
bool Teacher::isAuthorizedForClass(const std::string& className) const {
    return std::find(classesAssigned.begin(), classesAssigned.end(), className) != classesAssigned.end();
}

// Helper function to check if the teacher is assigned to a specific subject.
bool Teacher::isAuthorizedForSubject(const std::string& subjectName) const {
    for (const auto& s : subjectsTaught) {
        if (Utils::toLower(s) == Utils::toLower(subjectName)) {
            return true;
        }
    }
    return false;
}
void Teacher::manageAssignments() {
    Utils::clearScreen();
    std::cout << "--- Assignment Management ---\n\n";
    std::cout << "1. Upload a New Assignment\n";
    std::cout << "2. View Submission Status for an Assignment\n";
    std::cout << "0. Back to Main Menu\n";
    int choice = Utils::getValidatedInt("Your choice: ", 0, 2);

    if (choice == 1) {
        // --- UPLOAD NEW ASSIGNMENT ---
        std::cout << "\n--- Upload New Assignment ---\n";
        // 1. Get assignment details from teacher
        std::string className = Utils::getNonEmptyString("Enter class name (e.g., 9-A): ");
        if (!isAuthorizedForClass(className)) {
            std::cout << "Error: You are not authorized for this class.\n";
            return;
        }
        std::string subject = Utils::getNonEmptyString("Enter subject: ");
        std::string details = Utils::getNonEmptyString("Enter assignment details: ");
        std::string dueDate = Utils::getNonEmptyString("Enter due date (YYYY-MM-DD): ");

        // 2. Create the new assignment object
        auto assignments = FileManager::loadAssignments();
        Assignment new_assignment;
        new_assignment.assignmentID = time(0); // Simple unique ID
        new_assignment.teacherID = this->userID;
        new_assignment.className = className;
        new_assignment.subject = subject;
        new_assignment.details = details;
        new_assignment.dueDate = dueDate;
        assignments.push_back(new_assignment);
        FileManager::saveAssignments(assignments);

        // 3. Create a "Pending" submission record for every student in that class
        auto submissions = FileManager::loadSubmissions();
        std::ifstream student_file(FileManager::dataPath + "students.txt");
        std::string line;
        int students_assigned = 0;
        while(getline(student_file, line)) {
            auto tokens = Utils::split(line, ',');
            if (tokens.size() == 5 && tokens[3] == className) {
                submissions.push_back({new_assignment.assignmentID, tokens[0], "Pending", ""});
                students_assigned++;
            }
        }
        student_file.close();
        FileManager::saveSubmissions(submissions);
        
        std::cout << "\nAssignment uploaded successfully for " << students_assigned << " students in " << className << ".\n";

    } else if (choice == 2) {
        // --- VIEW SUBMISSION STATUS ---
        std::cout << "\n--- View Submission Status ---\n";
        auto all_assignments = FileManager::loadAssignments();
        std::vector<Assignment> my_assignments;
        for (const auto& a : all_assignments) {
            if (a.teacherID == this->userID) {
                my_assignments.push_back(a);
            }
        }

        if (my_assignments.empty()) {
            std::cout << "You have not uploaded any assignments.\n";
            return;
        }

        std::cout << "Select an assignment to view its status:\n";
        for (size_t i = 0; i < my_assignments.size(); ++i) {
            std::cout << i + 1 << ". [" << my_assignments[i].className << " - " << my_assignments[i].subject << "] " << my_assignments[i].details << "\n";
        }
        int assign_choice = Utils::getValidatedInt("Your choice: ", 1, my_assignments.size());
        int chosen_id = my_assignments[assign_choice - 1].assignmentID;

        auto all_submissions = FileManager::loadSubmissions();
        int submitted_count = 0, pending_count = 0;
        std::cout << "\n--- Submission Details ---\n";
        for (const auto& s : all_submissions) {
            if (s.assignmentID == chosen_id) {
                if (s.status == "Submitted" || s.status == "Late") submitted_count++;
                else pending_count++;
                std::cout << "Student: " << s.studentID << " | Status: " << s.status << "\n";
            }
        }
        std::cout << "\nSummary: " << submitted_count << " Submitted, " << pending_count << " Pending.\n";
    }
}