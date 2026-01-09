# include <iostream >
using namespace std;
# define SIZE 10
// Declare a stack class for characters .
class stack
{
    char stck [ SIZE ]; // holds the stack
    int tos ; // index of top of stack
public :
    stack (); // push character on stack
    char pop (); // pop character from stack
};
// Initialize the stack .
stack :: stack ()
{
    cout << " Constructing a stack \n";
    tos = 0;
}
// Push a character .
void stack :: push ( char ch)
{
    if(tos == SIZE )
    {
        cout << " Stack is full \n";
        return ;
    }
    stck [tos ] = ch;
    tos ++;
}
// Pop a character .
char stack :: pop ()
{
    if(tos ==0)
    {
        cout << " Stack is empty \n";
        return 0; // return null on empty stack
    }
    tos - -;
    return stck [ tos ];
}
int main ()
{ // Create two stacks that are automatically initialized .
    stack s1 , s2;
    int i;
    s1. push (’a’);
    s1. push (’b’);
    s1. push (’c’);
    // clone s1
    s2 = s1; // now s1 and s2 are identical
    for (i=0; i <3; i++)
        cout << "Pop s1: " << s1.pop () << "\n";
    for (i=0; i <3;i++)
        cout << "Pop s2: " << s2.pop () << "\n";
    return 0;
}