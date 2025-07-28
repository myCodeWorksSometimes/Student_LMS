# 🎓 Advanced Learning Management System (LMS) 🚀

A console-based, multi-role school management system designed to handle core academic and administrative workflows for 500+ students — all powered by flat text files! 📂📝

🏆 Objective
-------------
Build a rock‑solid LMS supporting Admins, Teachers, Students, Parents & Principal. Simulate real-world school processes and keep data safe with file-locking! 🔒

🔥 Key Features 🔥
------------------
✨ **Multi‑User Roles**  
  • Admin, Teacher, Student, Parent, Principal — each with tailored menus & permissions!  

💾 **Data Persistence**  
  • All data lives in `data/*.txt`. No DB needed!  

🔐 **Concurrency Guard**  
  • `lms.lock` ensures no two writes collide.  

👑 Role‑Specific Powers
-----------------------

👤 **Admin**  
  - 🆙 **Yearly Promotions**: Auto‑promote or retain students based on 50% average.  
  - 📊 **Workload Reports**: Teacher periods per week + custodian duties.  
  - 💰 **Salary Mgmt**: Pay staff, deduct for extra leaves, history log.  
  - 💸 **Fee Challans**: Issue, track overdue (>20 days ⏰), auto-cancel admissions.  
  - 🗂 **Student/Staff Records**: Add, view, edit, remove entries.  
  - 📤 **Leave Requests**: Forward teacher/custodian requests to Principal.  
  - 🗺 **Custodian Areas**: Assign & reassign cleaning zones.  
  - 🕒 **Timetable Checks**: Detect teacher/room clashes & weird schedules (no PE➡️ lunch!).  
  - 🧐 **Invigilation**: Auto‑assign available teachers, avoid self‑invigilation!  
  - 🚌 **Bus Routes** (Bonus): Nearest‑Neighbor heuristic magic!  

👩‍🏫 **Teacher**  
  - ✍ **Grade Mgmt**: Enter quiz/midterm/final — system weights 30/40/30%.  
  - 📑 **Term Reports**: One‑click class reports to `termReports.txt`.  
  - ✅ **Attendance**: Mark students & yourself daily.  
  - 🏖 **Apply Leave**: Formal leave with dates & reason.  
  - 📅 **My Timetable & Workload**: Weekly schedule + overload warnings.  
  - 📋 **Assignments**: Post new tasks, track statuses (Pending/Submitted/Late).  
  - 🍽 **Duty Roster**: See lunch or bus duty assignments.  

🎒 **Student**  
  - 🗓 **Timetable**: Check your weekly classes.  
  - 📚 **Assignments**: Due dates, status, mark as done ✅.  
  - 🏆 **Grades & Report Card**: All scores + overall average.  
  - 📈 **Attendance**: History & percentage.  
  - 🛍 **Canteen**: Balance & transaction history.  

👨‍👩‍👧 **Parent**  
  - 📊 **Progress Report**: Filterable child’s grades.  
  - 📆 **Attendance**: Child’s presence record.  
  - 💰 **Fees**: Challan status & payment history.  
  - ✉ **Section Change**: Request to Principal with reason.  
  - 📖 **Child’s Timetable**: Full weekly view.  

🎩 **Principal**  
  - ✅ **Approve Parent Requests**: Change‑of‑section & more.  
  - 🛂 **Approve Staff Leave**: From Admin’s referrals.  
  - 💵 **Set Salaries**: Initial & future increments.  
  - 📊 **School Dashboard**: Performance, attendance & workloads.  
  - 🔍 **Lookup**: Any student/staff record by ID.  

⚙️ Constraints & Magic Tricks
-----------------------------
- 🛡 **Data Integrity**: `lms.lock` file‑locking.  
- 🛑 **Input Validation**: Utils helpers (ints, non-empty strings).  
- 🔐 **Role‑Based Access**: Strict feature segregation.  
- ⚠ **Timetable Validation**: No double‑bookings or bad policies.  
- 🧮 **Weighted Grades**: 30% Quiz, 40% Midterm, 30% Final.  
- 📅 **Invigilation Scheduling**: Fair, no conflicts, respects leaves.  

📁 File Structure (`data/`)
---------------------------
Hundreds of `.txt` files: `admins.txt`, `students.txt`, `teachers.txt`, `attendance.txt`, `grades.txt`, `fee_challans.txt`, … see project docs.

🏃 How to Compile & Run 🏃‍♂️
-----------------------------
1. **Compile with FTXUI** (no CMake needed! Windows Tested mainly, but cross-platform):
   ```bash
    g++ main.cpp src/*.cpp -o lms_project -std=c++17 -I./headers/ -Iexternal/ftxui/include -Lexternal/ftxui/build -lftxui-component -lftxui-dom -lftxui-screen -pthread
