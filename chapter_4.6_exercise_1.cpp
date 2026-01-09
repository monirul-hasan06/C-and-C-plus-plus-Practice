#include <iostream>
using namespace std;

// Using pointer parameter
void neg_ptr(int* num) {
    *num = -(*num);
}

// Using reference parameter
void neg_ref(int& num) {
    num = -num;
}

int main() {
    int a = 10;
    int b = -5;

    cout << "Original values: a = " << a << ", b = " << b << endl;

    // Using pointer version
    neg_ptr(&a);
    neg_ptr(&b);
    cout << "After neg_ptr(): a = " << a << ", b = " << b << endl;

    // Using reference version
    neg_ref(a);
    neg_ref(b);
    cout << "After neg_ref(): a = " << a << ", b = " << b << endl;

    return 0;
}