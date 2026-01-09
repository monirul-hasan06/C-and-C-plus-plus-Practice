#include <iostream>

// Forward declarations
class pr2;

class pr1 {
    int printing;
public:
    pr1() { printing = 0; }
    void set_print(int status) { printing = status; }

    // Declare inuse() as a friend
    friend bool inuse(pr1 p1, pr2 p2);
};

class pr2 {
    int printing; // Added the private member 'printing' to pr2
public:
    pr2() { printing = 0; }
    void set_print(int status) { printing = status; }

    // Declare inuse() as a friend
    friend bool inuse(pr1 p1, pr2 p2);
};

// Definition of the friend function
bool inuse(pr1 p1, pr2 p2) {
    // Accessing private members of both classes
    return (p1.printing || p2.printing);
}

int main() {
    pr1 p1_obj;
    pr2 p2_obj;

    // Initially, both are not using the printer
    std::cout << "Is the printer in use? " << (inuse(p1_obj, p2_obj) ? "Yes" : "No") << std::endl;

    // pr1 starts using the printer
    p1_obj.set_print(1);
    std::cout << "pr1 is now using the printer." << std::endl;
    std::cout << "Is the printer in use? " << (inuse(p1_obj, p2_obj) ? "Yes" : "No") << std::endl;

    // pr2 starts using the printer (while pr1 is still using it)
    p2_obj.set_print(1);
    std::cout << "pr2 is now using the printer." << std::endl;
    std::cout << "Is the printer in use? " << (inuse(p1_obj, p2_obj) ? "Yes" : "No") << std::endl;

    // pr1 stops using the printer
    p1_obj.set_print(0);
    std::cout << "pr1 has finished." << std::endl;
    std::cout << "Is the printer in use? " << (inuse(p1_obj, p2_obj) ? "Yes" : "No") << std::endl;

    // pr2 stops using the printer
    p2_obj.set_print(0);
    std::cout << "pr2 has finished." << std::endl;
    std::cout << "Is the printer in use? " << (inuse(p1_obj, p2_obj) ? "Yes" : "No") << std::endl;

    return 0;
}