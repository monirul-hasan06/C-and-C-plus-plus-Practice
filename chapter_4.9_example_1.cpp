# include <iostream >
using namespace std;
int main ()
{
    int x;
    int &ref = x; // create an independent reference
    x = 10; // these two statements
    ref = 10; // are functionally equivalent
    ref = 100;
    // this prints the number 100 twice
    cout << x << ’ ’ << ref << "\n";
    return 0;
}
