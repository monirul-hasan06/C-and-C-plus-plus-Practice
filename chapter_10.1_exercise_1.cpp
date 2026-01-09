//
// Created by mithu on 11/9/2025.
//
#include <iostream>
using namespace std;

class base {
    int x;
public:
    void setx(int i) { x = i; }
    int getx() { return x; }
};

class derived : public base {
    int y;
public:
    void sety(int i) { y = i; }
    int gety() { return y; }
};

int main() {
    base b_ob;       // base class object
    derived d_ob;    // derived class object

    derived *p;      // pointer to derived type

    // p = &b_ob; // This will cause a compiler error

    // Correct: derived pointer can point only to derived objects
    p = &d_ob;

    p->setx(100);  // from base
    p->sety(200);  // from derived

    cout << "Derived object x: " << p->getx() << endl;
    cout << "Derived object y: " << p->gety() << endl;

    return 0;
}
