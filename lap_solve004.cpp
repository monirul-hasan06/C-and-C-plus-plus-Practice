// QUESTION 4 — Multilevel Inheritance (Employee → Department → Payroll)
#include <iostream>
using namespace std;

class Employee {
protected:
    string employeeName;
    int employeeID;
public:
    Employee(string name , int id = 0) {
        this->employeeName = name;
        this->employeeID = id;
    }
};

class Department : public Employee {
protected:
    string departmentName;
    string shiftTiming;
public:
    Department(string name, int id, string dname, string shift)
        : Employee(name, id)
    {
        this->departmentName = dname;
        this->shiftTiming = shift;
    }
};

class Payroll : public Department {
private:
    float basicSalary;
public:
    Payroll(string name, int id, string dname, string shift, float salary)
        : Department(name, id, dname, shift)
    {
        this->basicSalary = salary;
    }

    float calculateSalary() {
        if (shiftTiming == "Night")
            return basicSalary + (basicSalary * 0.10);
        return basicSalary;
    }

    void showProfile() {
        cout << "\n----- EMPLOYEE PROFILE -----\n";
        cout << "Name          : " << employeeName << endl;
        cout << "Employee ID   : " << employeeID << endl;
        cout << "Department    : " << departmentName << endl;
        cout << "Shift         : " << shiftTiming << endl;
        cout << "Basic Salary  : " << basicSalary << endl;
        cout << "Total Salary  : " << calculateSalary() << endl;
    }
};

int main() {
    Payroll p("Karim", 1201, "IT", "Night", 30000);
    p.showProfile();

    return 0;
}