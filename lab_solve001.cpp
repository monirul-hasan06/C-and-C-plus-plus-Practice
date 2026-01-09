//QUESTION 1 — Multiple Inheritance (Online Shopping Scenario)
#include <iostream>
using namespace std;

class Customer {
protected:
    string customerName, phone;
public:
    Customer(string name, string ph) {
        this->customerName = name;
        this->phone = ph;
    }
};

class Product {
protected:
    string productName;
    int productID;
public:
    Product(string pname , int pid = 0) {
        this->productName = pname;
        this->productID = pid;
    }
};

class Store {
protected:
    string storeName, location;
public:
    Store(string sname , string loc) {
        this->storeName = sname;
        this->location = loc;
    }
};

class OrderInfo : public Customer, public Product, public Store {
private:
    int quantity;
    string order_date;

public:
    OrderInfo(string cname, string ph, string pname, int pid,
              string sname, string loc, int qty, string date)
        : Customer(cname, ph), Product(pname, pid), Store(sname, loc)
    {
        this->quantity = qty;
        this->order_date = date;
    }

    void showOrder() {
        cout << "\n----- ORDER DETAILS -----\n";
        cout << "Customer Name  : " << customerName << endl;
        cout << "Phone Number   : " << phone << endl;
        cout << "Product Name   : " << productName << endl;
        cout << "Product ID     : " << productID << endl;
        cout << "Store Name     : " << storeName << endl;
        cout << "Location       : " << location << endl;
        cout << "Quantity       : " << quantity << endl;
        cout << "Order Date     : " << order_date << endl;
    }
};

int main() {
    OrderInfo order("Rahim", "01700000000", "Laptop", 101,
                    "TechShop", "Dhaka", 2, "08-12-2025");

    order.showOrder();
    return 0;
}