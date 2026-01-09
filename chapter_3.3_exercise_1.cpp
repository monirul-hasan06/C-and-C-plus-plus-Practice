#include <iostream>

class who {
private:
    char name;

public:
    // Constructor
    who(char n) : name(n) {
        std::cout << "Constructing who #" << name << std::endl;
    }

    // Destructor
    ~who() {
        std::cout << "Destroying who #" << name << std::endl;
    }

    // Member function to get the object's name
    char getName() {
        return name;
    }
};

// Function that returns a 'who' object
who make_who(char n) {
    who temp(n);
    return temp;
}

int main() {
    std::cout << "Starting main()...\n";

    // Call make_who(), which returns a 'who' object
    who object_a = make_who('A');

    std::cout << "Returned object is named #" << object_a.getName() << "\n";

    std::cout << "Ending main()...\n";

    return 0;
}