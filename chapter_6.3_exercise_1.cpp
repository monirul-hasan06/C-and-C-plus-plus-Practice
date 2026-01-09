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

    // Overload < operator (compare magnitude or sum of coordinates)
    bool operator<(const coord &ob) const {
        int sum1 = x + y;
        int sum2 = ob.x + ob.y;
        return sum1 < sum2;
    }

    // Overload > operator
    bool operator>(const coord &ob) const {
        int sum1 = x + y;
        int sum2 = ob.x + ob.y;
        return sum1 > sum2;
    }
};

int main() {
    coord c1(3, 5);
    coord c2(4, 2);

    cout << "c1 = ";
    c1.show();
    cout << "\nc2 = ";
    c2.show();
    cout << "\n\n";

    // Comparison using overloaded operators
    if (c1 > c2)
        cout << "c1 is greater than c2" << endl;
    else if (c1 < c2)
        cout << "c1 is less than c2" << endl;
    else
        cout << "c1 and c2 are equal" << endl;

    return 0;
}
