#include <iostream>
#include <stdexcept>
using namespace std;

class SafeArray {
private:
    int* data;
    int rows, cols;

public:
    SafeArray(int r, int c) : rows(r), cols(c) {
        data = new int[rows * cols];
        for(int i = 0; i < rows * cols; i++) {
            data[i] = 0;
        }
    }

    ~SafeArray() {
        delete[] data;
    }

    // Access using pointer arithmetic
    int* getPtr(int row, int col) {
        if(row < 0 || row >= rows || col < 0 || col >= cols) {
            throw out_of_range("Index out of bounds");
        }
        return data + (row * cols) + col;
    }

    void display() {
        for(int i = 0; i < rows; i++) {
            for(int j = 0; j < cols; j++) {
                cout << *getPtr(i, j) << " ";
            }
            cout << endl;
        }
    }
};

int main() {
    try {
        SafeArray arr(2, 3);

        // Access and modify using pointers
        *arr.getPtr(0, 0) = 1;
        *arr.getPtr(0, 1) = 2;
        *arr.getPtr(0, 2) = 3;
        *arr.getPtr(1, 0) = 4;
        *arr.getPtr(1, 1) = 5;
        *arr.getPtr(1, 2) = 6;

        cout << "2x3 Safe Array (pointer access):" << endl;
        arr.display();

        // Demonstrate pointer arithmetic
        int* ptr = arr.getPtr(0, 0);
        cout << "\nPointer traversal:" << endl;
        for(int i = 0; i < 6; i++) {
            cout << *(ptr + i) << " ";
        }
        cout << endl;

    } catch (const out_of_range& e) {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}