#pragma once
#include <string>
#include <vector>


struct AttendanceRecord {
    std::string studentID;
    std::string date;
    std::string className;
    std::string status;
    std::string teacherID;
    std::string subject;
};




struct LeaveRequest {
    int requestID;
    std::string employeeID;
    std::string employeeType;  // "Teacher" or "Custodian"
    std::string employeeName;
    std::string startDate;
    std::string endDate;
    int days;
    std::string reason;
    std::string status;
    std::string adminComments;
    std::string principalComments;

    LeaveRequest() = default;
    LeaveRequest(int id, const std::string& name, const std::string& type,
                 const std::string& fromDate, const std::string& toDate,
                 const std::string& status, const std::string& reason)
        : requestID(id), employeeName(name), employeeType(type), startDate(fromDate),
          endDate(toDate), status(status), reason(reason) {}
};


struct ParentRequest {
    int requestID;
    std::string parentID;
    std::string childID;
    std::string requestType;
    std::string details;
    std::string status;
};

struct StaffInfo {
    std::string id;
    std::string password;
    std::string name;
    std::string role;
    double salary;
};


struct TimetableSlot {
    std::string day;
    int period;
    std::string subject;
    std::string teacherID;
    std::string className;
    std::string room;
};

struct Grade {
    std::string studentID;
    std::string subject;
    double quiz;
    double midterm;
    double final;
    double total;      // Calculated weighted total
    std::string term;  // "Term1", "Term2", etc.
};


struct Assignment {
    int assignmentID;
    std::string teacherID;
    std::string className;
    std::string subject;
    std::string details;
    std::string dueDate; // Format: YYYY-MM-DD
};

struct Submission {
    int assignmentID;
    std::string studentID;
    std::string status; // e.g., "Pending", "Submitted", "Late"
    std::string submissionDate;
};