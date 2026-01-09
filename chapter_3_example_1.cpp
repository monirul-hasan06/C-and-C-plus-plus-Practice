# include <iostream >
using namespace std;
class myclass
{
    int a, b;
public :
void set (int i, int j) { a = i; b = j; }
    void show () { cout << a << ’ ’ << b << "\n"; }
};
/*
This class is similar to myclass but uses a
different class name and thus appears as a different
type to the compiler .
*/
class yourclass
{
    int a, b;
public :
void set (int i, int j) { a = i; b = j; }
    void show () { cout << a << ’ ’ << b << "\n"; }
};
int main ()
{
    myclass o1;
    yourclass o2;
    o1.set (10 , 4);
    o2 = o1; // ERROR , objects not of same type
    o1. show ();
    o2. show ();
    return 0;
}