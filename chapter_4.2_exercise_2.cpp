#include <iostream>
using namespace std;

int main() {
    int twoD[10][10];

    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 10; j++) {
            twoD[i][j] = (i+1) * (j+1);
        }
    }

    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 10; j++) {
            cout << twoD[i][j] << "\t";
        }
        cout << endl;
    }

    return 0;
}