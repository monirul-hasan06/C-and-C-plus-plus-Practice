#include <iostream>
using namespace std;

int main() {
    float* f = new float(3.14f);
    long* l = new long(1000000L);
    char* c = new char('A');

    cout << "Float: " << *f << endl;
    cout << "Long: " << *l << endl;
    cout << "Char: " << *c << endl;

    delete f;
    delete l;
    delete c;

    return 0;
}