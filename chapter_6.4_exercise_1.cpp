#include <iostream>
using namespace std;

class coord {
private:
    int x, y;

public:
    // Constructor
    coord(int a = 0, int b = 0) {
        x = a;
        y = b;
    }

    // Display function
    void show() const {
        cout << "(" << x << ", " << y << ")";
    }

    // Prefix -- operator: --obj
    coord operator--() {
        --x;
        --y;
        return *this; // return the updated object
    }

    // Postfix -- operator: obj--
    coord operator--(int) { // dummy int differentiates postfix
        coord temp = *this; // save original value
        x--;
        y--;
        return temp; // return old value
    }
};

int main() {
    coord c1(5, 10);

    cout << "Original c1 = ";
    c1.show();

    // Prefix form (--c1)
    coord c2 = --c1;
    cout << "\nAfter prefix --c1, c1 = ";
    c1.show();
    cout << "\nReturned value (c2) = ";
    c2.show();

    // Postfix form (c1--)
    coord c3 = c1--;
    cout << "\n\nAfter postfix c1--, c1 = ";
    c1.show();
    cout << "\nReturned value (c3) = ";
    c3.show();

    cout << endl;
    return 0;
}
