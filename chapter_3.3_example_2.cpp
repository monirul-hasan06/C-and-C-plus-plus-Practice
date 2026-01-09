# include <iostream >
# include <cstring >
# include <cstdlib >
using namespace std;
class samp
{
    char *s;
public :
samp () { s = ’\0 ’; }
    ~ samp () { if(s) free (s); cout << " Freeing s\n"; }
    void show () { cout << s << "\n"; }
    void set ( char *str );
};
// Load a string .
void samp :: set ( char *str )
{
    s = ( char *) malloc ( strlen (str)+1);
    if (!s)
    {
        cout << " Allocation error \n";
        exit (1) ;
    }
    strcpy (s, str);
}
// Return an object of type samp
samp input ()
{
    char s [80];
    samp str ;
    cout << " Enter a string : ";
    cin >> s;
    str .set (s);
    return str;
}
int main ()
{
    samp ob;
    // assign returned object to ob
    ob = input (); // This causes an error !!!!
    ob. show ();
    return 0;
}