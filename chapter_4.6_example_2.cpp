# include <iostream >
# include <cmath >
using namespace std;
void round ( double &num );
int main ()
{
    double i = 100.4;
    cout << i << " rounded is ";
    round (i);
    cout << i << "\n";
    i = 10.9;
    cout << i << " rounded is ";
    round (i);
    cout << i << "\n";
    return 0;
}
void round ( double &num )
{
    double frac ;
    double val;
    // decompose num into whole and fractional parts
    frac = modf (num , &val );
    if( frac < 0.5)
        num =val ;
    else
        num = val +1.0;

}