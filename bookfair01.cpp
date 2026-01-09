#include <iostream>
#include <string>
using namespace std;

// ---------------------------
//       Book Class
// ---------------------------
class Book {
public:
    string name;
    string author;
    int availableCopies;
    double price;

    // Default constructor
    Book() {
        name = "";
        author = "";
        availableCopies = 0;
        price = 0.0;
    }

    // Parameterized constructor
    Book(string n, string a, int copies, double p) {
        name = n;
        author = a;
        availableCopies = copies;
        price = p;
    }

    // Copy constructor
    Book(const Book &b) {
        name = b.name;
        author = b.author;
        availableCopies = b.availableCopies;
        price = b.price;
    }

    // Function to show book details
    void showBook() {
        cout << "Book Name: " << name
             << " | Author: " << author
             << " | Copies: " << availableCopies
             << " | Price: " << price << endl;
    }
};

// ---------------------------
//     Bookstall Class
// ---------------------------
class Bookstall {
public:
    string name;
    string address;
    string contactInfo;
    double balance;
    Book bookList[5]; // 5 books per stall

    // Default constructor
    Bookstall() {
        name = "";
        address = "";
        contactInfo = "";
        balance = 0.0;
    }

    // Parameterized constructor
    Bookstall(string n, string a, string c) {
        name = n;
        address = a;
        contactInfo = c;
        balance = 0.0;
    }

    // Copy constructor
    Bookstall(const Bookstall &b) {
        name = b.name;
        address = b.address;
        contactInfo = b.contactInfo;
        balance = b.balance;
        for (int i = 0; i < 5; i++) {
            bookList[i] = b.bookList[i];
        }
    }

    // Function to show stall info
    void showStall() {
        cout << "\n=== Book Stall: " << name << " ===" << endl;
        cout << "Address: " << address << " | Contact: " << contactInfo << endl;
        cout << "Balance: " << balance << endl;
        cout << "Books in this stall:" << endl;
        for (int i = 0; i < 5; i++) {
            bookList[i].showBook();
        }
    }

    // Function to sell a book
    bool sellBook(int index, Book &soldBook) {
        if (index >= 0 && index < 5 && bookList[index].availableCopies > 0) {
            bookList[index].availableCopies--;
            soldBook = bookList[index];
            balance += bookList[index].price;
            return true;
        } else {
            return false;
        }
    }
};

// ---------------------------
//     Customer Class
// ---------------------------
class Customer {
public:
    string name;
    string nationalID;
    double wallet;
    string deliveryAddress;
    string contactInfo;
    Book purchasedBooks[10];
    int totalPurchased;

    // Default constructor
    Customer() {
        name = "";
        nationalID = "";
        wallet = 0.0;
        deliveryAddress = "";
        contactInfo = "";
        totalPurchased = 0;
    }

    // Parameterized constructor
    Customer(string n, string nid, double w, string addr, string contact) {
        name = n;
        nationalID = nid;
        wallet = w;
        deliveryAddress = addr;
        contactInfo = contact;
        totalPurchased = 0;
    }

    // Copy constructor
    Customer(const Customer &c) {
        name = c.name;
        nationalID = c.nationalID;
        wallet = c.wallet;
        deliveryAddress = c.deliveryAddress;
        contactInfo = c.contactInfo;
        totalPurchased = c.totalPurchased;
        for (int i = 0; i < totalPurchased; i++) {
            purchasedBooks[i] = c.purchasedBooks[i];
        }
    }

    // Function to buy a book from a stall
    void buyBook(Bookstall &stall, int bookIndex) {
        Book temp;
        if (stall.bookList[bookIndex].price <= wallet && stall.bookList[bookIndex].availableCopies > 0) {
            if (stall.sellBook(bookIndex, temp)) {
                wallet -= temp.price;
                purchasedBooks[totalPurchased] = temp;
                totalPurchased++;
                cout << name << " bought \"" << temp.name << "\" from " << stall.name << endl;
            }
        } else {
            cout << name << " cannot buy that book (not enough money or no copies left)." << endl;
        }
    }

    // Function to show customer info
    void showCustomer() {
        cout << "\n+++ Customer: " << name << " +++" << endl;
        cout << "NID: " << nationalID << " | Wallet: " << wallet << endl;
        cout << "Address: " << deliveryAddress << " | Contact: " << contactInfo << endl;
        cout << "Purchased Books (" << totalPurchased << "):" << endl;
        for (int i = 0; i < totalPurchased; i++) {
            purchasedBooks[i].showBook();
        }
    }
};

// ---------------------------
//           MAIN
// ---------------------------
int main() {
    // Create 3 bookstalls
    Bookstall stall1("Stall A", "North Zone", "0101");
    Bookstall stall2("Stall B", "South Zone", "0102");
    Bookstall stall3("Stall C", "East Zone", "0103");

    // Add 5 books to each stall
    stall1.bookList[0] = Book("C++ Basics", "Lippman", 5, 50);
    stall1.bookList[1] = Book("OOP in C++", "Stroustrup", 5, 55);
    stall1.bookList[2] = Book("Algorithms", "Cormen", 5, 70);
    stall1.bookList[3] = Book("Data Structure", "Weiss", 5, 60);
    stall1.bookList[4] = Book("Programming Logic", "Yashwant", 5, 45);

    stall2.bookList[0] = Book("Python 101", "Eric", 5, 40);
    stall2.bookList[1] = Book("Java Made Easy", "Bloch", 5, 50);
    stall2.bookList[2] = Book("Clean Code", "Martin", 5, 65);
    stall2.bookList[3] = Book("SQL Guide", "Alan", 5, 35);
    stall2.bookList[4] = Book("Web Dev", "Kevin", 5, 55);

    stall3.bookList[0] = Book("AI Intro", "Russell", 5, 70);
    stall3.bookList[1] = Book("Machine Learning", "Andrew", 5, 75);
    stall3.bookList[2] = Book("Networks", "Tanenbaum", 5, 60);
    stall3.bookList[3] = Book("Operating Systems", "Stallings", 5, 65);
    stall3.bookList[4] = Book("Database", "Silberschatz", 5, 55);

    // Create customers
    Customer c1("Alice", "NID01", 200, "Dhaka", "0171");
    Customer c2("Bob", "NID02", 150, "Khulna", "0172");

    // Each customer buys 1 book from each stall
    c1.buyBook(stall1, 0);
    c1.buyBook(stall2, 1);
    c1.buyBook(stall3, 2);

    c2.buyBook(stall1, 4);
    c2.buyBook(stall2, 0);
    c2.buyBook(stall3, 3);

    // Show final results
    stall1.showStall();
    stall2.showStall();
    stall3.showStall();

    c1.showCustomer();
    c2.showCustomer();

    return 0;
}

