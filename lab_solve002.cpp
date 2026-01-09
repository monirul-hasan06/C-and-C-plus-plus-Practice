//QUESTION 2 — Template Class Box (Generic Volume Calculation)
#include <iostream>
using namespace std;

template <class T>
class Box {
private:
    T length, width, height;
public:
    Box(T l, T w, T h) {
        this->length = l;
        this->width = w;
        this->height = h;
    }

    T volume() {
        return this->length * this->width * this->height;
    }
};

int main() {
    Box<int> b1(3, 4, 5);
    Box<float> b2(2.5, 1.5, 3.2);

    cout << "Volume (int box): " << b1.volume() << endl;
    cout << "Volume (float box): " << b2.volume() << endl;

    return 0;
}