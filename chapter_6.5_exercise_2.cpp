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

    // Display
    void show() {
        cout << "(" << x << ", " << y << ")" << endl;
    }

    // Friend functions for multiplication
    friend coord operator*(coord c, int n);  // coord * int
    friend coord operator*(int n, coord c);  // int * coord
};

// -----------------------------
// Operator Overloading (friend)
// -----------------------------

// coord * int
coord operator*(coord c, int n) {
    coord temp;
    temp.x = c.x * n;
    temp.y = c.y * n;
    return temp;
}

// int * coord
coord operator*(int n, coord c) {
    coord temp;
    temp.x = n * c.x;
    temp.y = n * c.y;
    return temp;
}

int main() {
    coord a(2, 5), result;

    cout << "a = "; a.show();

    // coord * int
    result = a * 3;
    cout << "a * 3 = "; result.show();

    // int * coord
    result = 4 * a;
    cout << "4 * a = "; result.show();

    return 0;
}
