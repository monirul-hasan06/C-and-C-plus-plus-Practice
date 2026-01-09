#include <iostream>
using namespace std;

class Student {
private:
    string name;
    string id;
    double age;
    double cgpa;

public:
    // Default constructor
    Student() {
        name = "Mithu";
        id = "087";
        age = 25;
        cgpa = 2.61;
    }

    // Parameterized setter function
    void SetData(string n, string i, double a, double c) {
        name = n;
        id = i;
        age = a;
        cgpa = c;
    }

    // Display function
    void display() {
        cout << "Name: " << name << endl;
        cout << "ID: " << id << endl;
        cout << "Age: " << age << endl;
        cout << "CGPA: " << cgpa << endl;
    }
};

int main() {
    Student ob;       // Calls default constructor
    ob.display();     // Displays default data

    cout << "Hello World!" << endl;

    return 0;
}
