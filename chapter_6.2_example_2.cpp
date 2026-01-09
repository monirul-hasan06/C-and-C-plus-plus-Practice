// Overload the + , -, and = relative to coord class .
# include <iostream >
using namespace std;
class coord
{
    int x, y; // coordinate values
public :
coord () { x=0; y=0; };
    coord (int i, int j) { x=i; y=j; }
    void get_xy (int &i, int &j) { i=x; j=y; }
    coord operator +( coord ob2);
    coord operator -( coord ob2);
    coord operator =( coord ob2);
};
// Overload + relative to coord class .
coord coord :: operator +( coord ob2)
{
    coord temp ;
    temp .x = x + ob2 .x;
    temp .y = y + ob2 .y;
    return temp ;
}
// Overload - relative to coord class .
coord coord :: operator -( coord ob2)
{
    coord temp ;
    temp .x = x - ob2 .x;
    temp .y = y - ob2 .y;
    return temp ;
}
// Overload = relative to coord class .
coord coord :: operator =( coord ob2)
{
    x = ob2.x;
    y = ob2.y;
    return * this ; // return the object that is assigned
}
int main ()
{
    coord o1 (10 , 10) , o2 (5 , 3) , o3;
    int x, y;
    o3 = o1 + o2; // add two objects - this calls operator +
    o3. get_xy (x, y);
    cout << "(o1+o2) X: " << x << ", Y: " << y << "\n";
    o3 = o1 - o2; // subtract two objects
    o3. get_xy (x, y);
    cout << "(o1 -o2) X: " << x << ", Y: " << y << "\n";
    o3 = o1; // assign an object
    o3. get_xy (x, y);
    cout << "(o3=o1) X: " << x << ", Y: " << y << "\n";
    return 0;
}