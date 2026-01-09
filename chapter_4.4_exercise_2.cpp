#include <iostream>
#include <string>
using namespace std;

class Person {
public:
    string name;
    string phone;

    Person(string n, string p) : name(n), phone(p) {}
};

int main() {
    Person* person = new Person("Your Name", "123-456-7890");

    cout << "Name: " << person->name << endl;
    cout << "Phone: " << person->phone << endl;

    delete person;

    return 0;
}