#include <iostream>
using namespace std;

#define SIZE 100

class stack {
    int stk[SIZE];
    int tos;
public:
    void init();
    void push(int i);
    int pop();

    // Declaration of the friend function
    friend void showstack(stack o);
};

void stack::init() {
    tos = 0;
}

void stack::push(int i) {
    if(tos == SIZE) {
        cout << "Stack is full.\n";
        return;
    }
    stk[tos] = i;
    tos++;
}

int stack::pop() {
    if(tos == 0) {
        cout << "Stack underflow.\n";
        return 0;
    }
    tos--;
    return stk[tos];
}

// Definition of the friend function
void showstack(stack o) {
    cout << "Contents of the stack:" << endl;
    for(int i = 0; i < o.tos; i++) {
        cout << o.stk[i] << " ";
    }
    cout << endl;
}

int main() {
    stack s1, s2;

    s1.init();
    s2.init();

    s1.push(10);
    s1.push(20);
    s1.push(30);

    s2.push(1);
    s2.push(2);
    s2.push(3);

    cout << "Displaying contents of s1:" << endl;
    showstack(s1);

    cout << "\nDisplaying contents of s2:" << endl;
    showstack(s2);

    return 0;
}