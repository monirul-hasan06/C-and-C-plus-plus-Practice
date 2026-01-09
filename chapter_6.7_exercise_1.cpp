#include <iostream>
#include <cstring>
#include <cstdlib> // for exit()
using namespace std;

class strtype {
    char *p;   // pointer to string
    int len;   // length of string

public:
    // Constructor
    strtype(const char *s) {
        len = strlen(s);
        p = new char[len + 1];
        strcpy(p, s);
    }

    // Destructor
    ~strtype() {
        delete[] p;
    }

    // Overload [] operator
    char &operator[](int i) {
        if (i < 0 || i >= len) {
            cout << "Index out of range!" << endl;
            exit(1);
        }
        return p[i]; // return reference so we can modify it
    }

    // Display string
    void show() {
        cout << p << endl;
    }
};

int main() {
    strtype s1("Hello");
    cout << "Original string: ";
    s1.show();

    // Access characters using []
    cout << "s1[1] = " << s1[1] << endl;

    // Modify characters using []
    s1[1] = 'a';
    s1[4] = '!';

    cout << "Modified string: ";
    s1.show();

    // Example of invalid index (will stop program)
    // cout << s1[10];  // Uncomment to test error message

    return 0;
}
