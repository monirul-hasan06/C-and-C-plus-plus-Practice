# include <iostream >
using namespace std;
int &f(); // return a reference
int x;
int main ()
{
    f() = 100; // assign 100 to reference returned by f()
    cout << x << "\n";
    return 0;
}
// Return an int reference
int &f()
{
    return x; // returns a reference to x
}