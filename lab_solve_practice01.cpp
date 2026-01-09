#include <iostream>
using namespace std;

/*---------------------------
      Base Class: Employee
----------------------------*/
class Employee {
public:
    string employeeName;
    int employeeId;

    void getEmployeeInfo(string name, int id) {
        employeeName = name;
        employeeId = id;
    }
};

/*--------------------------------
   Derived Class 1: Department
---------------------------------*/
class Department : public Employee {
public:
    string departmentName;
    string shiftTiming;

    void getDepartmentInfo(string dept, string shift) {
        departmentName = dept;
        shiftTiming = shift;
    }
};

/*-------------------------------
   Derived Class 2: Payroll
-------------------------------*/
class Payroll : public Department {
public:
    float basicSalary;

    void getSalary(float salary) {
        basicSalary = salary;
    }

    float calculateTotalSalary() {
        // Add 10% bonus for Night shift
        if (shiftTiming == "Night" || shiftTiming == "night") {
            return basicSalary + (basicSalary * 0.10);
        }
        return basicSalary;
    }

    void showProfile() {
        cout << "\n===== Employee Full Profile =====\n";
        cout << "Employee Name   : " << employeeName << endl;
        cout << "Employee ID     : " << employeeId << endl;

        cout << "Department Name : " << departmentName << endl;
        cout << "Shift Timing    : " << shiftTiming << endl;

        cout << "Basic Salary    : " << basicSalary << endl;
        cout << "Total Salary    : " << calculateTotalSalary() << endl;
    }
};

/*-------------------------
          main()
--------------------------*/
int main() {
    Payroll emp;

    emp.getEmployeeInfo("Abdul Karim", 1205);
    emp.getDepartmentInfo("IT", "Night");
    emp.getSalary(30000);

    emp.showProfile();

    return 0;
}
