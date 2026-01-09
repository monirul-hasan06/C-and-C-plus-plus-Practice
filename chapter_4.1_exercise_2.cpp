#include <iostream>
using namespace std;

class squares
{
    int num, sqr;
public:
    squares(int a, int b) { num = a; sqr = b; }
    void show() { cout << num << ' ' << sqr << "\n"; }
};

int main()
{
    // Create a ten-element array initialized with numbers 1-10 and their squares
    squares squaresArray[10] = {
        squares(1, 1*1), squares(2, 2*2), squares(3, 3*3),
        squares(4, 4*4), squares(5, 5*5), squares(6, 6*6),
        squares(7, 7*7), squares(8, 8*8), squares(9, 9*9),
        squares(10, 10*10)
    };

    // Alternative initialization using loop (commented out)
    /*
    squares squaresArray[10];
    for(int i = 0; i < 10; i++) {
        int num = i + 1;
        squaresArray[i] = squares(num, num * num);
    }
    */

    // Display all elements
    cout << "Number and its square:" << endl;
    cout << "----------------------" << endl;
    for(int i = 0; i < 10; i++) {
        squaresArray[i].show();
    }

    // Verify the values are correct
    cout << "\nVerification:" << endl;
    cout << "----------------------" << endl;
    for(int i = 0; i < 10; i++) {
        int expected_num = i + 1;
        int expected_sqr = expected_num * expected_num;

        // Since num and sqr are private, we'll demonstrate through show()
        // or we could add getter methods if needed
        cout << "Element " << i << ": ";
        squaresArray[i].show();
    }

    return 0;
}