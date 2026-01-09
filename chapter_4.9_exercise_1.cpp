#include <iostream>
using namespace std;

// Return reference to enable direct modification and method chaining
class Counter {
    int count;
public:
    Counter() : count(0) {}

    Counter& increment() {
        count++;
        return *this; // Return reference for chaining
    }

    Counter& decrement() {
        count--;
        return *this;
    }

    int get() const { return count; }
};

int main() {
    Counter c;
    // Method chaining using reference returns
    c.increment().increment().decrement().increment();
    cout << "Count: " << c.get() << endl;

    return 0;
}