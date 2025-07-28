
#pragma once
#include "User.h"
#include "DataModels.h" // For TimetableSlot
#include <vector>
#include <string>

class Admin : public User {
public:
    Admin(std::string id, std::string pass, std::string n);
    void displayMenu() override;

private:
    
    void salaryManagement();      
    void feeManagement();         
    void manageStudentRecords();
    // void cancelStudentAdmission();
    // Main Feature Functions (called from displayMenu)
    void yearlyClassAllocation();
    void staffWorkloadBalancing();
    // void paymentManagement();
    // void manageStudentRecords();
    void cancelStudentAdmission();
    void manageStaffRecords();
    void forwardLeaveRequests();
    void manageCustodianAssignments();
    void validateTimetable();
    void scheduleExamInvigilators();
    void optimizeTransportRoutes(); // Bonus Feature

    // Sub-menu for Payment Management
    void paySalaries();
    void issueFeeChallans();
    void checkOverdueFeesAndCancel();
    void viewPaymentReports();

    // Sub-menu for Student Record Management
    void addNewStudent();
    void viewStudentRecords();
    void editStudentRecord();
    void removeStudentRecord();

    // Sub-menu for Staff Record Management
    void addNewStaff();
    void viewStaffRecords();
    void updateStaffRecord();

    // Sub-menu for Leave Requests
    void forwardTeacherLeaveRequests();
    void createCustodianLeaveRequest();
    void viewAllPendingLeaveRequests();

    // Sub-menu for Custodian Management
    void assignCustodianArea();
    void viewCustodianAssignments();
    void reassignCustodianArea();
    void addNewCustodianStaff();

    // Helper Functions
    void cancelStudentAdmissionById(const std::string& student_id);
    double getSalaryForEmployee(const std::string& employee_id, const std::string& type);
    double calculateLeaveDeduction(const std::string& employee_id);
    void rewriteFile(const std::string& filename, const std::vector<std::string>& lines);
};