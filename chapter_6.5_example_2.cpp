// Use friend operator functions to add flexibility .
# include <iostream >
using namespace std;
class coord
{
    int x, y; // coordinate values
public :
coord () { x=0; y=0; };
    coord (int i, int j) { x=i; y=j; }
    void get_xy (int &i, int &j) { i=x; j=y; }
    friend coord operator +( coord ob1 , int i);
    friend coord operator +( int i, coord ob1 );
};
// Overload + for ob + int
coord operator +( coord ob1 , int i)
{
    coord temp ;
    temp .x = ob1 .x + i;
    temp .y = ob1 .y + i;
    return temp ;
}
// Overload + for int + ob
coord operator +( int i, coord ob1)
{
    coord temp ;
    temp .x = ob1 .x + i;
    temp .y = ob1 .y + i;
    return temp ;
}
int main ()
{
    coord o1 (10 , 10);
    int x, y;
    o1 = o1 + 10; // object + integer
    o1. get_xy (x, y);
    cout << "(o1 +10) X: " << x << ", Y: " << y << "\n";
    o1 = 99 + o1; // integer + object
    o1. get_xy (x, y);
    cout << " (99+ o1) X: " << x << ", Y: " << y << "\n";
    return 0;
}