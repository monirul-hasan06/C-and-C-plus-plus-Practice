#include <iostream>
#include <cstring>
using namespace std;

int main() {
    char *p;
    p = new char[100];  // Dynamically allocate array of 100 chars

    strcpy(p, "This is a test");
    cout << p << endl;

    delete[] p;  // Release the allocated memory

    return 0;
}