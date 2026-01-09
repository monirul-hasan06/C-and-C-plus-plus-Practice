#include <iostream>
using namespace std;

// Reference version (cleaner)
void swap_ref(int &a, int &b) {
    int temp = a;
    a = b;
    b = temp;
}

// Pointer version (more complex)
void swap_ptr(int *a, int *b) {
    if (a && b) { // Null check needed
        int temp = *a;
        *a = *b;
        *b = temp;
    }
}

int main() {
    int x = 5, y = 10;
    
    // Reference version - cleaner and safer
    swap_ref(x, y);
    
    // Pointer version - more verbose
    swap_ptr(&x, &y);
    
    return 0;
}