#include <iostream>
using namespace std;

class cl2; // Forward declaration of cl2

class cl1 {
    int i, j;
public:
    cl1(int a, int b) { i = a; j = b; }

    // Overloaded assignment operator to allow assignment from cl2
    cl1& operator=(const cl2& obj);

    void display() {
        cout << "cl1 object: i = " << i << ", j = " << j << endl;
    }
};

class cl2 {
    int i, j;
public:
    cl2(int a, int b) { i = a; j = b; }

    // Friend declaration to allow cl1 to access cl2's private members
    friend cl1& cl1::operator=(const cl2& obj);

    void display() {
        cout << "cl2 object: i = " << i << ", j = " << j << endl;
    }
};

// Definition of the overloaded assignment operator
cl1& cl1::operator=(const cl2& obj) {
    this->i = obj.i;
    this->j = obj.j;
    return *this;
}

int main() {
    cl1 x(10, 20);
    cl2 y(0, 0);

    cout << "Before assignment:" << endl;
    x.display();
    y.display();

    // Assignment is now valid
    x = y;

    cout << "\nAfter assignment (x = y):" << endl;
    x.display();
    y.display();

    return 0;
}