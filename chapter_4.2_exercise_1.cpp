#include <iostream>
using namespace std;

class squares {
    int num, sqr;
public:
    squares(int a, int b) { num = a; sqr = b; }
    void show() { cout << num << ' ' << sqr << "\n"; }
};

int main() {
    squares ob[10] = {
        squares(1, 1),
        squares(2, 4),
        squares(3, 9),
        squares(4, 16),
        squares(5, 25),
        squares(6, 36),
        squares(7, 49),
        squares(8, 64),
        squares(9, 81),
        squares(10, 100)
    };

    // Display in reverse order (from last element to first)
    cout << "Array contents in reverse order:" << endl;
    cout << "-------------------------------" << endl;
    for(int i = 9; i >= 0; i--) {
        ob[i].show();
    }

    return 0;
}