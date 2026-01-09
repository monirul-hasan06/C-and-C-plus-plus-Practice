#include <iostream>
using namespace std;

class coord {
private:
    int x, y;

public:
    coord(int a = 0, int b = 0) {
        x = a;
        y = b;
    }

    // ❌ Wrong return type: int instead of coord
    int operator+(coord ob) {
        return x + ob.x + y + ob.y;
    }

    void show() {
        cout << "(" << x << ", " << y << ")";
    }
};

int main() {
    coord c1(2, 3), c2(4, 5), c3;

    // ❌ Error: invalid conversion from ‘int’ to ‘coord’
    c3 = c1 + c2;

    cout << "\nResult = ";
    c3.show();
    cout << endl;

    return 0;
}
