# include <iostream >
using namespace std;
class truck ; // a forward declaration
class car
{
    int passengers ;
    int speed ;
public :
car (int p, int s) { passengers = p; speed = s; }
    int sp_greater ( truck t);
};
class truck
{
    int weight ;
    int speed ;
public :
truck (int w, int s) { weight = w; speed = s; }
    // note new use of the scope resolution operator
    friend int car :: sp_greater ( truck t);
};
/*
Return positive if car speed faster than truck .
Return 0 if speeds are the same .
Return negative if truck speed faster than car .
*/
int car :: sp_greater ( truck t)
{
    /*
    Since sp_greater () is member of car , only a
    truck object must be passed to it.
    */
    return speed - t. speed ;
}
int main ()
{
    int t;
    car c1 (6 , 55) , c2 (2 , 120) ;
    truck t1 (10000 , 55) , t2 (20000 , 72);
    cout << " Comparing c1 and t1 :\n";
    t = c1. sp_greater (t1); // evoke as member function of car
    if(t <0)
        cout << " Truck is faster .\n";
    else if(t ==0)
        cout << "Car and truck speed is the same .\n";
    else
        cout << "Car is faster .\n";
    cout << " Comparing c2 and t2 :\n";
    t = c2. sp_greater (t2); // evoke as member function of car
    if(t <0)
        cout << " Truck is faster .\n";
    else if(t ==0)
        cout << "Car and truck speed is the same .\n";
    else
        cout << "Car is faster .\n";
    return 0;
}