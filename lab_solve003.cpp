// QUESTION 3 — Exception Handling for Student Marks
#include <iostream>
#include <stdexcept>
using namespace std;

int main() {
    int marks[3];

    for (int i = 0; i < 3; i++) {
        try {
            cout << "Enter mark for subject " << i + 1 << ": ";
            cin >> marks[i];

            if (marks[i] < 0 || marks[i] > 100) {
                throw runtime_error("Invalid mark entered: " + to_string(marks[i]));
            }
        }
        catch (runtime_error &e) {
            cout << e.what() << endl;
            i--;  // Retry input
        }
    }

    cout << "\nValid Marks Entered:\n";
    for (int i = 0; i < 3; i++)
        cout << "Subject " << i + 1 << ": " << marks[i] << endl;

    return 0;
}