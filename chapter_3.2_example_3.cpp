# include <iostream >
using namespace std;
class samp
{
    int i;
public :
samp (int n) { i = n; }
    void set_i (int n) { i = n; }
    int get_i () { return i; }
};
/*
Set o.i to its square . This affects the calling
argument .
*/
void sqr_it ( samp *o)
{
    o- > set_i (o- > get_i () * o- > get_i ());
    cout << " Copy of a has i value of " << o- > get_i ();
    cout << "\n";
}
int main ()
{
    samp a (10) ;
    sqr_it (&a); // pass a’s address of sqr_it ()
    cout << "Now , a in main () has been changed : ";
    cout << a. get_i (); // displays 100
    return 0;
}