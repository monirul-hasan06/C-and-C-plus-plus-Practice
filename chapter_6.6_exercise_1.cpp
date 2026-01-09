#include <iostream>
#include <cstdlib> // for exit()
using namespace std;

class dynarray {
    int *p;    // pointer to dynamic array
    int size;  // size of the array

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

    // Copy assignment operator
    dynarray& operator=(const dynarray &obj) {
        // Avoid self-assignment
        if (this == &obj)
            return *this;

        // Delete old memory
        delete[] p;

        // Copy size and allocate new memory
        size = obj.size;
        p = new int[size];

        // Copy all elements
        for (int i = 0; i < size; i++)
            p[i] = obj.p[i];

        return *this;
    }

    // Return reference to element i (for assignment)
    int& put(int i) {
        if (i < 0 || i >= size) {
            cout << "Index out of bounds!" << endl;
            exit(1);
        }
        return p[i];
    }

    // Return value of element i
    int get(int i) {
        if (i < 0 || i >= size) {
            cout << "Index out of bounds!" << endl;
            exit(1);
        }
        return p[i];
    }

    // Destructor to free memory
    ~dynarray() {
        delete[] p;
    }
};

int main() {
    dynarray a(5);  // create dynamic array of size 5

    // Store values using put()
    for (int i = 0; i < 5; i++)
        a.put(i) = i * 10;

    // Display values using get()
    cout << "Array a: ";
    for (int i = 0; i < 5; i++)
        cout << a.get(i) << " ";
    cout << endl;

    // Use assignment operator
    dynarray b(5);
    b = a;

    cout << "Array b (after assignment): ";
    for (int i = 0; i < 5; i++)
        cout << b.get(i) << " ";
    cout << endl;

    return 0;
}

