#include <iostream>
using namespace std;

class letters
{
    char ch;
public:
    letters(char c) { ch = c; }
    char get_ch() { return ch; }
};

int main()
{
    // Create a ten-element array initialized with A through J
    letters letterArray[10] = {
        letters('A'), letters('B'), letters('C'), letters('D'), letters('E'),
        letters('F'), letters('G'), letters('H'), letters('I'), letters('J')
    };

    // Alternative initialization using loop (commented out)
    /*
    letters letterArray[10];
    for(int i = 0; i < 10; i++) {
        letterArray[i] = letters('A' + i);
    }
    */

    // Demonstrate the array contains values A through J
    cout << "Array contents: ";
    for(int i = 0; i < 10; i++) {
        cout << letterArray[i].get_ch();
        if(i < 9) cout << ", ";
    }
    cout << endl;

    // Verify each element individually
    cout << "\nVerification:" << endl;
    for(int i = 0; i < 10; i++) {
        char expected = 'A' + i;
        char actual = letterArray[i].get_ch();
        cout << "Element " << i << ": Expected '" << expected
             << "', Got '" << actual << "' - "
             << (expected == actual ? "✓ MATCH" : "✗ MISMATCH") << endl;
    }

    return 0;
}