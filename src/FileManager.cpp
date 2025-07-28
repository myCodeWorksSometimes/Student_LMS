#include "../headers/FileManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cstdio> 
#include <vector>
#include "../headers/Utils.h"

const std::string FileManager::dataPath = "data/";

//Concurrency kia hota hai zamin?

// --- Lock Management Functions ---

// This function attempts to create a lock file.
// It returns true on success and false if the lock is already held.
bool create_lock() {
    std::ifstream lockfile(FileManager::dataPath + "lms.lock");
    if (lockfile.good()) {
        // The lock file already exists, so another process is writing.
        std::cerr << "ERROR: System is busy. Another process is writing to data files.\n"
                  << "Please try again in a moment.\n";
        Utils::pause(); // Pause to let the user see the message
        return false;
    }
    // The lock file doesn't exist, so we can acquire the lock.
    std::ofstream outfile(FileManager::dataPath + "lms.lock");
    outfile << "locked"; // Write something to the file
    outfile.close();
    return true;
}

// This function releases the lock by deleting the lock file.
void release_lock() {
    remove((FileManager::dataPath + "lms.lock").c_str());
}


// --- User Loading (Read Operation - No Lock Needed) ---

std::vector<std::unique_ptr<User>> FileManager::loadAllUsers() {
    std::vector<std::unique_ptr<User>> users;
    std::ifstream file;
    std::string line;

    // Load Students
    file.open(dataPath + "students.txt");
    if (file.is_open()) {
        while (getline(file, line)) {
            auto tokens = Utils::split(line, ',');
            if (tokens.size() == 5) {
                users.push_back(std::make_unique<Student>(tokens[0], tokens[1], tokens[2], tokens[3], tokens[4]));
            }
        }
        file.close();
    } else { std::cerr << "Warning: Could not open students.txt\n"; }
    
    // Load Teachers
    file.open(dataPath + "teachers.txt");
    if (file.is_open()) {
        while (getline(file, line)) {
            auto tokens = Utils::split(line, ',');
            if (tokens.size() == 6) {
                auto subjects = Utils::split(tokens[4], ';');
                auto classes = Utils::split(tokens[5], ';');
                users.push_back(std::make_unique<Teacher>(tokens[0], tokens[1], tokens[2], subjects, classes));
            }
        }
        file.close();
    } else { std::cerr << "Warning: Could not open teachers.txt\n"; }

    // Load Parents
    file.open(dataPath + "parents.txt");
    if (file.is_open()) {
        while (getline(file, line)) {
            auto tokens = Utils::split(line, ',');
            if (tokens.size() == 4) {
                users.push_back(std::make_unique<Parent>(tokens[0], tokens[1], tokens[2], tokens[3]));
            }
        }
        file.close();
    } else { std::cerr << "Warning: Could not open parents.txt\n"; }

    // Load Admins
    file.open(dataPath + "admins.txt");
    if (file.is_open()) {
        while (getline(file, line)) {
            auto tokens = Utils::split(line, ',');
            if (tokens.size() == 3) {
                users.push_back(std::make_unique<Admin>(tokens[0], tokens[1], tokens[2]));
            }
        }
        file.close();
    } else { std::cerr << "Warning: Could not open admins.txt\n"; }

    // Load Principals
    file.open(dataPath + "principals.txt");
    if (file.is_open()) {
        while (getline(file, line)) {
            auto tokens = Utils::split(line, ',');
            if (tokens.size() == 3) {
                users.push_back(std::make_unique<Principal>(tokens[0], tokens[1], tokens[2]));
            }
        }
        file.close();
    } else { std::cerr << "Warning: Could not open principals.txt\n"; }
    
    return users;
}


// --- Data Loading Functions (Read Operations - No Lock Needed) ---

std::vector<Grade> FileManager::loadGrades() {
    std::vector<Grade> grades;
    std::ifstream file(dataPath + "grades.txt");
    std::string line;
    while(getline(file, line)) {
        auto tokens = Utils::split(line, ',');
        if(tokens.size() == 5) {
            Grade g;
            g.studentID = tokens[0];
            g.subject = tokens[1];
            g.quiz = std::stod(tokens[2]);
            g.midterm = std::stod(tokens[3]);
            g.final = std::stod(tokens[4]);
            // Weighting: Quiz 30%, Midterm 40%, Final 30%
            g.total = (g.quiz * 0.3) + (g.midterm * 0.4) + (g.final * 0.3);
            grades.push_back(g);
        }
    }
    file.close();
    return grades;
}

std::vector<AttendanceRecord> FileManager::loadAttendance() {
    std::vector<AttendanceRecord> records;
    std::ifstream file(dataPath + "attendance.txt");
    std::string line;
    while(getline(file, line)) {
        auto tokens = Utils::split(line, ',');
        if (tokens.size() == 4) { // Original simpler format
            records.push_back({tokens[0], tokens[1], tokens[2], tokens[3], "", ""});
        } else if (tokens.size() == 6) { // More detailed format
            records.push_back({tokens[0], tokens[1], tokens[2], tokens[3], tokens[4], tokens[5]});
        }
    }
    file.close();
    return records;
}

std::vector<LeaveRequest> FileManager::loadLeaveRequests() {
    std::vector<LeaveRequest> requests;
    std::ifstream file(dataPath + "leave_requests.txt");
    std::string line;
    while(getline(file, line)) {
        auto tokens = Utils::split(line, ',');
        if (tokens.size() >= 7) { // Support optional fields
            LeaveRequest req;
            req.requestID = std::stoi(tokens[0]);
            req.employeeID = tokens[1];
            req.employeeName = tokens[2];
            req.employeeType = tokens[3];
            req.startDate = tokens[4];
            req.endDate = tokens[5];
            req.reason = tokens[6];
            if (tokens.size() > 7) req.days = std::stoi(tokens[7]);
            if (tokens.size() > 8) req.status = tokens[8];
            if (tokens.size() > 9) req.adminComments = tokens[9];
            if (tokens.size() > 10) req.principalComments = tokens[10];
            requests.push_back(req);
        }
    }
    file.close();
    return requests;
}

std::vector<TimetableSlot> FileManager::loadTimetable() {
    std::vector<TimetableSlot> timetable;
    std::ifstream file(dataPath + "timetable.txt");
    std::string line;
    while(getline(file, line)) {
        auto tokens = Utils::split(line, ',');
        if(tokens.size() == 6) {
            TimetableSlot ts;
            ts.day = tokens[0];
            ts.period = std::stoi(tokens[1]);
            ts.className = tokens[2];
            ts.room = tokens[3];
            ts.subject = tokens[4];
            ts.teacherID = tokens[5];
            timetable.push_back(ts);
        }
    }
    file.close();
    return timetable;
}

// --- Data Saving Functions (Write Operations - LOCKING IS CRITICAL) ---

void FileManager::saveGrades(const std::vector<Grade>& grades) {
    if (!create_lock()) return;
    
    std::ofstream file(dataPath + "grades.txt");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open grades.txt for writing.\n";
        release_lock();
        return;
    }
    for (const auto& g : grades) {
        file << g.studentID << "," << g.subject << "," << g.quiz << "," << g.midterm << "," << g.final << "\n";
    }
    file.close();
    release_lock();
}

void FileManager::saveAttendance(const std::vector<AttendanceRecord>& attendance) {
    if (!create_lock()) return;
    
    // We always append to attendance, not overwrite
    std::ofstream file(dataPath + "attendance.txt", std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open attendance.txt for appending.\n";
        release_lock();
        return;
    }
    for(const auto& rec : attendance) {
         file << rec.studentID << "," << rec.date << "," << rec.className << "," 
              << rec.status << "," << rec.teacherID << "," << rec.subject << "\n";
    }
    file.close();
    release_lock();
}

void FileManager::saveLeaveRequests(const std::vector<LeaveRequest>& requests) {
    if (!create_lock()) return;

    std::ofstream file(dataPath + "leave_requests.txt");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open leave_requests.txt for writing.\n";
        release_lock();
        return;
    }
    for(const auto& req : requests) {
        // Saving all fields for robustness
        file << req.requestID << "," << req.employeeID << "," << req.employeeName << ","
             << req.employeeType << "," << req.startDate << "," << req.endDate << ","
             << req.reason << "," << req.days << "," << req.status << ","
             << req.adminComments << "," << req.principalComments << "\n";
    }
    file.close();
    release_lock();
}


// --- New functions for assignment tracking (LOCKING IS CRITICAL) ---

std::vector<Assignment> FileManager::loadAssignments() {
    std::vector<Assignment> assignments;
    std::ifstream file(dataPath + "assignments.txt");
    std::string line;
    while(getline(file, line)) {
        auto tokens = Utils::split(line, ',');
        if(tokens.size() == 6) {
            assignments.push_back({
                std::stoi(tokens[0]), tokens[1], tokens[2], 
                tokens[3], tokens[4], tokens[5]
            });
        }
    }
    file.close();
    return assignments;
}

void FileManager::saveAssignments(const std::vector<Assignment>& assignments) {
    if (!create_lock()) return;
    
    std::ofstream file(dataPath + "assignments.txt");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open assignments.txt for writing.\n";
        release_lock();
        return;
    }
    for (const auto& a : assignments) {
        file << a.assignmentID << "," << a.teacherID << "," << a.className << ","
             << a.subject << "," << a.details << "," << a.dueDate << "\n";
    }
    file.close();
    release_lock();
}

std::vector<Submission> FileManager::loadSubmissions() {
    std::vector<Submission> submissions;
    std::ifstream file(dataPath + "submissions.txt");
    std::string line;
    while(getline(file, line)) {
        auto tokens = Utils::split(line, ',');
        if(tokens.size() == 4) {
            submissions.push_back({std::stoi(tokens[0]), tokens[1], tokens[2], tokens[3]});
        }
    }
    file.close();
    return submissions;
}

void FileManager::saveSubmissions(const std::vector<Submission>& submissions) {
    if (!create_lock()) return;
    
    std::ofstream file(dataPath + "submissions.txt");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open submissions.txt for writing.\n";
        release_lock();
        return;
    }
    for (const auto& s : submissions) {
        file << s.assignmentID << "," << s.studentID << "," 
             << s.status << "," << s.submissionDate << "\n";
    }
    file.close();
    release_lock();
}


// --- Generic File I/O (Read doesn't need lock, Append does) ---

void FileManager::readAndDisplayFile(const std::string& filename) {
    std::ifstream file(dataPath + filename);
    if (!file.is_open()) {
        std::cout << "Could not open " << filename << ". It may not exist yet.\n";
        return;
    }
    std::string line;
    while (getline(file, line)) {
        std::cout << line << std::endl;
    }
    file.close();
}

void FileManager::appendToReport(const std::string& report) {
    if (!create_lock()) return;
    
    std::ofstream file(dataPath + "termReports.txt", std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open termReports.txt for appending.\n";
        release_lock();
        return;
    }
    file << report << "\n\n";
    file.close();
    release_lock();
}