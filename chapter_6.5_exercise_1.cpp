#include <iostream>
using namespace std;

class coord {
    int x, y;

public:
    // Constructors
    coord() {
        x = 0;
        y = 0;
    }

    coord(int a, int b) {
        x = a;
        y = b;
    }

    // Show coordinates
    void show() {
        cout << "(" << x << ", " << y << ")" << endl;
    }

    // Friend functions to overload - and /
    friend coord operator-(coord c1, coord c2);
    friend coord operator/(coord c1, coord c2);
};

// -----------------------------
// Operator Overloading (friend)
// -----------------------------

// Subtraction (-)
coord operator-(coord c1, coord c2) {
    coord temp;
    temp.x = c1.x - c2.x;
    temp.y = c1.y - c2.y;
    return temp;
}

// Division (/)
coord operator/(coord c1, coord c2) {
    coord temp;
    // Avoid division by zero
    temp.x = (c2.x != 0) ? c1.x / c2.x : 0;
    temp.y = (c2.y != 0) ? c1.y / c2.y : 0;
    return temp;
}

int main() {
    coord a(20, 10), b(5, 2), c;

    cout << "a = "; a.show();
    cout << "b = "; b.show();

    // Subtraction
    c = a - b;
    cout << "a - b = "; c.show();

    // Division
    c = a / b;
    cout << "a / b = "; c.show();

    return 0;
}
