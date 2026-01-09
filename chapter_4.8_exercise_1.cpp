#include <iostream>
#include <stdexcept>
using namespace std;

class SafeArray {
private:
    int* data;
    int rows, cols;

    // Private function to check bounds
    bool isValid(int row, int col) const {
        return (row >= 0 && row < rows && col >= 0 && col < cols);
    }

public:
    // Constructor
    SafeArray(int r, int c) : rows(r), cols(c) {
        data = new int[rows * cols];
        // Initialize to zeros
        for(int i = 0; i < rows * cols; i++) {
            data[i] = 0;
        }
    }

    // Destructor
    ~SafeArray() {
        delete[] data;
    }

    // Overloaded [] operator for row access
    class RowProxy {
    private:
        int* rowStart;
        int cols;
    public:
        RowProxy(int* start, int c) : rowStart(start), cols(c) {}

        // Overloaded [] operator for column access
        int& operator[](int col) {
            if(col < 0 || col >= cols) {
                throw out_of_range("Column index out of bounds");
            }
            return rowStart[col];
        }

        // Const version for read-only access
        const int& operator[](int col) const {
            if(col < 0 || col >= cols) {
                throw out_of_range("Column index out of bounds");
            }
            return rowStart[col];
        }
    };

    // Access operator
    RowProxy operator[](int row) {
        if(row < 0 || row >= rows) {
            throw out_of_range("Row index out of bounds");
        }
        return RowProxy(data + (row * cols), cols);
    }

    // Const version for read-only access
    const RowProxy operator[](int row) const {
        if(row < 0 || row >= rows) {
            throw out_of_range("Row index out of bounds");
        }
        return RowProxy(data + (row * cols), cols);
    }

    // Display the array
    void display() const {
        for(int i = 0; i < rows; i++) {
            for(int j = 0; j < cols; j++) {
                cout << (*this)[i][j] << " ";
            }
            cout << endl;
        }
    }
};

int main() {
    try {
        // Create 2x3 safe array
        SafeArray arr(2, 3);

        // Fill with values
        arr[0][0] = 1;
        arr[0][1] = 2;
        arr[0][2] = 3;
        arr[1][0] = 4;
        arr[1][1] = 5;
        arr[1][2] = 6;

        cout << "2x3 Safe Array contents:" << endl;
        arr.display();

        // Demonstrate bounds checking
        cout << "\nTesting bounds checking:" << endl;

        // These should work
        cout << "Valid access - arr[1][2]: " << arr[1][2] << endl;

        // These should throw exceptions
        try {
            cout << "Invalid row access: ";
            cout << arr[2][0] << endl; // Should throw
        } catch (const out_of_range& e) {
            cout << "Caught exception: " << e.what() << endl;
        }

        try {
            cout << "Invalid column access: ";
            cout << arr[0][3] << endl; // Should throw
        } catch (const out_of_range& e) {
            cout << "Caught exception: " << e.what() << endl;
        }

        // Test negative indices
        try {
            cout << "Negative index access: ";
            cout << arr[-1][0] << endl; // Should throw
        } catch (const out_of_range& e) {
            cout << "Caught exception: " << e.what() << endl;
        }

    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}