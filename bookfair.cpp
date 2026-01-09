#include <iostream>
#include <string>
using namespace std;

// Forward declaration
class Book;

// ---------------------------
//       Book Class
// ---------------------------
class Book {
private:
    string name;
    string author;
    int availableCopies;
    double price;

public:
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

    // Friend function
    friend void showBook(const Book &b);

    // Inline accessors
    string getName() const { return name; }
    string getAuthor() const { return author; }
    int getCopies() const { return availableCopies; }
    double getPrice() const { return price; }

    // Sell a copy
    bool sellBook() {
        if (availableCopies > 0) {
            availableCopies--;
            return true;
        }
        return false;
    }
};

// Friend function definition
void showBook(const Book &b) {
    cout << "Book: " << b.name << " | Author: " << b.author
         << " | Copies: " << b.availableCopies
         << " | Price: " << b.price << endl;
}

// ---------------------------
//     Bookstall Class
// ---------------------------
class Bookstall {
private:
    string name;
    string address;
    string contactInfo;
    double balance;
    Book bookList[5]; // Each stall has 5 books

public:
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
    Bookstall(const Bookstall &bs) {
        name = bs.name;
        address = bs.address;
        contactInfo = bs.contactInfo;
        balance = bs.balance;
        for (int i = 0; i < 5; i++)
            bookList[i] = bs.bookList[i];
    }

    // Add a book
    void setBook(int i, Book b) {
        if (i >= 0 && i < 5)
            bookList[i] = b;
    }

    // Sell a book by index
    bool sellBook(int i, Book &soldBook) {
        if (i >= 0 && i < 5 && bookList[i].sellBook()) {
            soldBook = bookList[i];
            balance += bookList[i].getPrice();
            return true;
        }
        return false;
    }

    double getBalance() { return balance; }

    void showDetails() {
        cout << "\n=== Stall: " << name << " ===" << endl;
        cout << "Address: " << address << " | Contact: " << contactInfo << endl;
        cout << "Balance: " << balance << endl;
        cout << "Books:" << endl;
        for (int i = 0; i < 5; i++)
            showBook(bookList[i]);
    }

    // Friend access for Customer
    friend class Customer;
};

// ---------------------------
//     Customer Class
// ---------------------------
class Customer {
private:
    string name;
    string nationalID;
    double wallet;
    string deliveryAddress;
    string contactInfo;
    Book purchasedBooks[10]; // can buy up to 10 books
    int totalPurchased;

public:
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
        for (int i = 0; i < totalPurchased; i++)
            purchasedBooks[i] = c.purchasedBooks[i];
    }

    // Member function: show personal info
    void showPersonalInfo() {
        cout << "\n+++ Customer: " << name << " +++" << endl;
        cout << "NID: " << nationalID << " | Wallet: " << wallet << endl;
        cout << "Address: " << deliveryAddress << " | Contact: " << contactInfo << endl;
        cout << "Purchased Books: " << totalPurchased << endl;
        for (int i = 0; i < totalPurchased; i++)
            showBook(purchasedBooks[i]);
    }

    // Member function: visit stall
    void visitBookStall(Bookstall &stall) {
        cout << "\n" << name << " visiting " << stall.name << endl;
        for (int i = 0; i < 5; i++) {
            Book temp;
            if (stall.bookList[i].getCopies() > 0 && stall.bookList[i].getPrice() <= wallet) {
                if (stall.sellBook(i, temp)) {
                    wallet -= temp.getPrice();
                    purchasedBooks[totalPurchased++] = temp;
                    cout << name << " bought \"" << temp.getName()
                         << "\" for " << temp.getPrice() << endl;
                    return;
                }
            }
        }
        cout << name << " couldn't buy any book from " << stall.name << endl;
    }

    // Member function: purchase
    void purchase(Book b) {
        if (wallet >= b.getPrice()) {
            wallet -= b.getPrice();
            purchasedBooks[totalPurchased++] = b;
        }
    }
};

// ---------------------------
//         MAIN
// ---------------------------
int main() {
    // Create 3 bookstalls
    Bookstall stalls[3] = {
        Bookstall("Stall A", "North Wing", "010-1111"),
        Bookstall("Stall B", "South Wing", "010-2222"),
        Bookstall("Stall C", "East Wing", "010-3333")
    };

    // Add 5 books to each stall
    stalls[0].setBook(0, Book("C++ Primer", "Lippman", 5, 50));
    stalls[0].setBook(1, Book("Clean Code", "Martin", 5, 45));
    stalls[0].setBook(2, Book("Design Patterns", "Gamma", 5, 60));
    stalls[0].setBook(3, Book("Algorithms", "Cormen", 5, 70));
    stalls[0].setBook(4, Book("Pragmatic Programmer", "Hunt", 5, 40));

    stalls[1].setBook(0, Book("Python Crash Course", "Eric", 5, 35));
    stalls[1].setBook(1, Book("Effective Java", "Bloch", 5, 55));
    stalls[1].setBook(2, Book("Refactoring", "Fowler", 5, 50));
    stalls[1].setBook(3, Book("Head First C++", "Freeman", 5, 38));
    stalls[1].setBook(4, Book("Learn SQL", "Alan", 5, 30));

    stalls[2].setBook(0, Book("AI", "Russell", 5, 65));
    stalls[2].setBook(1, Book("Networks", "Tanenbaum", 5, 60));
    stalls[2].setBook(2, Book("DBMS", "Silberschatz", 5, 55));
    stalls[2].setBook(3, Book("OS", "Stallings", 5, 58));
    stalls[2].setBook(4, Book("Deep Learning", "Goodfellow", 5, 80));

    // Create 5 customers
    Customer customers[5] = {
        Customer("Alice", "NID01", 200, "Dhaka", "0171001"),
        Customer("Bob", "NID02", 150, "Khulna", "0171002"),
        Customer("Cathy", "NID03", 180, "Sylhet", "0171003"),
        Customer("David", "NID04", 300, "Rajshahi", "0171004"),
        Customer("Eve", "NID05", 100, "Chittagong", "0171005")
    };

    // Each customer visits all 3 stalls
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 3; j++) {
            customers[i].visitBookStall(stalls[j]);
        }
    }

    // Show final report
    cout << "\n\n=== FINAL STALL DETAILS ===" << endl;
    for (int i = 0; i < 3; i++)
        stalls[i].showDetails();

    cout << "\n\n=== FINAL CUSTOMER DETAILS ===" << endl;
    for (int i = 0; i < 5; i++)
        customers[i].showPersonalInfo();

    return 0;
}
