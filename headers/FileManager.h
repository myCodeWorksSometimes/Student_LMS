#pragma once
#include <vector>
#include <string>
#include <memory>
#include <set>
#include "User.h"
#include "DataModels.h"
#include "Student.h"
#include "Teacher.h"
#include "Parent.h"
#include "Admin.h"
#include "Principal.h"

class FileManager {
public:
    static const std::string dataPath;
    
    // Existing functions (keep these)
    static std::vector<std::unique_ptr<User>> loadAllUsers();
    static std::vector<Grade> loadGrades();
    static std::vector<AttendanceRecord> loadAttendance(); //man
    static std::vector<TimetableSlot> loadTimetable();
    static std::vector<LeaveRequest> loadLeaveRequests();
    
    static void saveGrades(const std::vector<Grade>& grades);
    static void saveAttendance(const std::vector<AttendanceRecord>& attendance);
    static void saveLeaveRequests(const std::vector<LeaveRequest>& requests);
    
    static void readAndDisplayFile(const std::string& filename);
    static void appendToReport(const std::string& report);
    
    // Additional functions you might need to implement in FileManager.cpp
    // (Only if not already implemented)
    static void saveTimetable(const std::vector<TimetableSlot>& timetable);
    static std::vector<std::string> loadStudentRecords();
    static void saveStudentRecords(const std::vector<std::string>& students);
    static std::vector<Assignment> loadAssignments();
    static void saveAssignments(const std::vector<Assignment>& assignments);
    static std::vector<Submission> loadSubmissions();
    static void saveSubmissions(const std::vector<Submission>& submissions);
};

