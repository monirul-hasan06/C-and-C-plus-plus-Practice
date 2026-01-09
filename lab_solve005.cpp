/*C++ provides a built-in exception handling mechanism to manage run-time errors. It uses three keywords:

1. try
    Contains statements that may generate an error.


    Any code that might throw an exception must be inside a try block.


    2. throw
    Used to signal/raise an exception.


    Can throw any data type (int, float, string, object, etc.).


    3. catch
    Handles the exception thrown by throw.


    Must appear immediately after the try block.


    Multiple catch blocks are allowed; the correct one runs based on data type of the exception.


    How it works
    Code inside try executes.


    If an error occurs → throw sends the exception.


    The matching catch block handles it.


    Remaining catch blocks are skipped.

*/
Example 1:
    # include <iostream >
    using namespace std;
int main ()
{
    cout << " start \n";
    try // start a try block
    {
        cout << " Inside try block \n";
        throw 10; // throw an error
        cout << " This will not execute ";
    }
    catch (int i) // catch an error
    {
        cout << " Caught One ! Number is: ";
        cout << i << "\n";
    }
    cout << "end ";
    return 0;
}


Example 2:


#include <iostream>
using namespace std;

int main()
{
    cout << "Program Started\n";

    try
    {
        cout << "Inside try block\n";
        throw "An error occurred!";   // throwing a string
        cout << "This line will not run\n";
    }
    catch (const char* msg)
    {
        cout << "Exception Caught: " << msg << endl;
    }

    cout << "Program Ended\n";
    return 0;
}