// Overload the + for ob + int as well as ob + ob.
# include <iostream >
using namespace std;
class coord
{
    int x, y; // coordinate values
public :
coord () { x=0; y=0; };
    coord (int i, int j) { x=i; y=j; }
    void get_xy (int &i, int &j) { i=x; j=y; }
    coord operator +( coord ob2); // ob + ob
    coord operator +( int i); // ob + int
};
// Overload + relative to coord class .
coord coord :: operator +( coord ob2)
{
    coord temp ;
    temp .x = x + ob2 .x;
    temp .y = y + ob2 .y;
    return temp ;
}
// Overload + for ob + int
coord coord :: operator +( int i)
{
    coord temp ;
    temp .x = x + i;
    temp .y = y + i;
    return temp ;
}
int main ()
{
    coord o1 (10 , 10) , o2 (5 , 3) , o3;
    int x, y;
    o3 = o1 + o2; // add two objects - calls operator +( coord )
    o3. get_xy (x, y);
    cout << "(o1+o2) X: " << x << ", Y: " << y << "\n";
    o3 = o1 + 100; // add object + int - calls operator +( int )
    o3. get_xy (x, y);
    cout << "(o1 +100) X: " << x << ", Y: " << y << "\n";
    return 0;
}