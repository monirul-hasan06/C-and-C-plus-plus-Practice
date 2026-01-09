#include <iostream>
using namespace std;

int global = 10;

int &f() {
    return global;  // Return reference to global variable
}

int main() {
    // INVALID: Cannot assign reference to pointer
    // int *x;
    // x = f();  // Error: cannot convert 'int' to 'int*'

    // VALID: Get address of referenced variable
    int *x = &f();
    cout << *x << endl;  // Output: 10

    // VALID: Use reference directly
    int &y = f();
    cout << y << endl;   // Output: 10

    return 0;
}