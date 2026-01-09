#include<iostream>
using namespace std;
    class Student{
    public:
    string name;
    int age;
    string grade;
};
int main(){
    Student s1;
    s1.name = "Mithu";
    s1.age = 22;
    s1.grade = "A+";
    cout<< s1.name;
    return 0;
};

