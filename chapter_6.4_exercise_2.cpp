#include <iostream>
using namespace std;

class coord {
    int x, y;

public:
    // Default constructor
    coord() {
        x = 0;
        y = 0;
    }

    // Parameterized constructor
    coord(int a, int b) {
        x = a;
        y = b;
    }

    // Display function
    void show() {
        cout << "(" << x << ", " << y << ")" << endl;
    }

    // Binary + operator (adds two coord objects)
    coord operator+(coord obj) {
        coord temp;
        temp.x = x + obj.x;
        temp.y = y + obj.y;
        return temp;
    }

    // Unary + operator (makes negative values positive)
    coord operator+() {
        coord temp;
        temp.x = (x < 0) ? -x : x;
        temp.y = (y < 0) ? -y : y;
        return temp;
    }
};

int main() {
    coord a(-5, 10);
    coord b(15, -20);
    coord c;

    cout << "a = "; a.show();
    cout << "b = "; b.show();

    // Binary +
    c = a + b;
    cout << "a + b = "; c.show();

    // Unary +
    c = +a;
    cout << "+a = "; c.show();

    return 0;
}
