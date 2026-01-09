#include <iostream>
#include <cstdlib> // for exit()
using namespace std;

class dynarray {
    int *p;     // pointer to dynamic array
    int size;   // size of array

public:
    // Constructor: allocate memory
    dynarray(int s) {
        size = s;
        p = new int[size];
        if (!p) {
            cout << "Memory allocation failed!" << endl;
            exit(1);
        }
    }

    // Overload assignment operator (deep copy)
    dynarray& operator=(const dynarray &obj) {
        if (this == &obj) // self-assignment check
            return *this;

        delete[] p;  // free old memory
        size = obj.size;
        p = new int[size];
        for (int i = 0; i < size; i++)
            p[i] = obj.p[i];
        return *this;
    }

    // Overload [] operator
    int& operator[](int i) {
        if (i < 0 || i >= size) {
            cout << "Index out of bounds!" << endl;
            exit(1);
        }
        return p[i]; // return reference for read/write access
    }

    // Destructor
    ~dynarray() {
        delete[] p;
    }
};

int main() {
    dynarray a(5);

    // Assign values using []
    for (int i = 0; i < 5; i++)
        a[i] = i * 10;

    // Display values using []
    cout << "Array a: ";
    for (int i = 0; i < 5; i++)
        cout << a[i] << " ";
    cout << endl;

    // Test assignment operator
    dynarray b(5);
    b = a;

    cout << "Array b (after assignment): ";
    for (int i = 0; i < 5; i++)
        cout << b[i] << " ";
    cout << endl;

    // Example of invalid index (uncomment to test)
    // cout << a[10];

    return 0;
}

