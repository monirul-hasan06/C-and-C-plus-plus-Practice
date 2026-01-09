#include <iostream>
#include <new> // Required for std::bad_alloc and std::nothrow
using namespace std;

int main() {
    // Method 1: Throws std::bad_alloc exception (default)
    try {
        int* huge_array = new int[1000000000000]; // Extremely large allocation
        delete[] huge_array;
    }
    catch (bad_alloc& e) {
        cout << "Allocation failed: " << e.what() << endl;
    }

    // Method 2: Returns null pointer (with nothrow)
    int* huge_array2 = new(nothrow) int[1000000000000];
    if (huge_array2 == nullptr) {
        cout << "Allocation failed: returned null pointer" << endl;
    }
    else {
        delete[] huge_array2;
    }

    return 0;
}