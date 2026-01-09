#include <iostream>
#include <cmath> // for fmod()
using namespace std;

class coord {
private:
    double x, y;

public:
    // Constructor
    coord(double a = 0, double b = 0) {
        x = a;
        y = b;
    }

    // Display coordinates
    void show() const {
        cout << "(" << x << ", " << y << ")";
    }

    // Overload % operator to find remainder (like a % b)
    coord operator%(const coord &ob) const {
        coord temp;
        temp.x = fmod(x, ob.x);  // fmod() works with floating-point numbers
        temp.y = fmod(y, ob.y);
        return temp;
    }
};

int main() {
    coord c1(10.5, 25.8);
    coord c2(3.0, 4.5);
    coord c3;

    cout << "First coordinate c1 = ";
    c1.show();
    cout << "\nSecond coordinate c2 = ";
    c2.show();

    // Use overloaded % operator
    c3 = c1 % c2;

    cout << "\n\nResult of c1 % c2 = ";
    c3.show();
    cout << endl;

    return 0;
}
