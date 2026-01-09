// A safe array example .
# include <iostream >
# include <cstdlib >
using namespace std;
const int SIZE = 5;
class arraytype
{
    int a[ SIZE ];
public :
arraytype ()
{
    int i;
    for (i=0; i< SIZE ; i++)
        a[i] = i;

}
    int & operator []( int i);
};
// Provide range checking for arraytype .
int & arraytype :: operator []( int i)
{
    if(i <0 || i>SIZE -1)
    {
        cout << "\ nIndex value of ";
        cout << i << " is out of bounds .\n";
        exit (1) ;
    }
    return a[i];
}
int main ()
{
    arraytype ob;
    int i;
    // this is OK
    for (i=0; i< SIZE ; i++)
        cout << ob[i] << " ";
    /*
    this generates a run - time error because
    SIZE +100 is out of range
    */
    ob[ SIZE +100] = 99; // error
    return 0;
}