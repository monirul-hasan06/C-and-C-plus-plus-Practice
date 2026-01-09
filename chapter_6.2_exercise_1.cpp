#include <iostream>
using namespace std;

class Coord {
private:
    float x, y;

public:
    // Constructor
    Coord(float a = 0, float b = 0) {
        x = a;
        y = b;
    }

    // Display function
    void display() const {
        cout << "(" << x << ", " << y << ")" << endl;
    }

    // Overload * operator (coordinate to coordinate)
    Coord operator*(const Coord& c) const {
        return Coord(x * c.x, y * c.y);
    }

    // Overload / operator (coordinate to coordinate)
    Coord operator/(const Coord& c) const {
        return Coord(x / c.x, y / c.y);
    }

    // Overload * operator (coordinate to scalar)
    Coord operator*(float value) const {
        return Coord(x * value, y * value);
    }

    // Overload / operator (coordinate to scalar)
    Coord operator/(float value) const {
        return Coord(x / value, y / value);
    }
};

int main() {
    Coord c1(4, 8);
    Coord c2(2, 4);

    cout << "c1 = "; c1.display();
    cout << "c2 = "; c2.display();

    // Coordinate to coordinate multiplication
    Coord c3 = c1 * c2;
    cout << "\nc1 * c2 = "; c3.display();

    // Coordinate to coordinate division
    Coord c4 = c1 / c2;
    cout << "c1 / c2 = "; c4.display();

    // Coordinate to scalar multiplication
    Coord c5 = c1 * 2.5;
    cout << "\nc1 * 2.5 = "; c5.display();

    // Coordinate to scalar division
    Coord c6 = c1 / 2;
    cout << "c1 / 2 = "; c6.display();

    return 0;
}
