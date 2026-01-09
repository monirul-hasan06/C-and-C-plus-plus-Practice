# include <iostream >
using namespace std;
int main ()
{
    int *p;
    p = new int (9); // give initial value of 9
    if (!p)
    {
        cout << " Allocation error \n";
        return 1;
    }
    cout << " Here is integer at p: " << *p << "\n";
    delete p; // release memory
    return 0;
}