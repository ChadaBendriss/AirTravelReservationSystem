#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <thread>
#include <vector>
#include <mutex>

using namespace std;

class Person {
protected:
    string name;

public:
    Person() {}
    Person(const string& n) : name(n) {}
    string getName() const { return name; }
};

class Customer : public Person {
    static int cusCount;
    int cusID;
    int age;
    int mobileNo;
    string address;
    string gender;

public:
    Customer() {}
    Customer(const string& n, int a, int m, const string& ad, const string& g)
        : Person(n), age(a), mobileNo(m), address(ad), gender(g) {
        cusID = ++cusCount;
    }

    void getDetails();
    void showDetails() const;
    bool fetchCustomer(const string& searchName);
};

int Customer::cusCount = 0;

void Customer::getDetails() {
    cout << "Enter Name: ";
    cin.ignore();
    getline(cin, name);
    cout << "Enter Age: ";
    cin >> age;
    cout << "Enter Mobile Number: ";
    cin >> mobileNo;
    cin.ignore();
    cout << "Enter Address: ";
    getline(cin, address);
    cout << "Enter Gender: ";
    cin >> gender;
    cusID = ++cusCount;

    ofstream out("old-customers.txt", ios::app);
    out << "\nCustomer ID: " << cusID << "\nName: " << name << "\nAge: " << age
        << "\nMobile Number: " << mobileNo << "\nAddress: " << address << "\nGender: " << gender << endl;
    out.close();

    cout << "\nSaved \nNOTE: We save your details record for future purposes\n"
         << endl;
}

void Customer::showDetails() const {
    ifstream in("old-customers.txt");
    if (!in) {
        cout << "File Error!" << endl;
        return;
    }
    string line;
    while (getline(in, line)) {
        cout << line << endl;
    }
    in.close();
}

bool Customer::fetchCustomer(const string& searchName) {
    ifstream in("old-customers.txt");
    string line;
    bool found = false;

    // Skip the first line if it exists
    if (getline(in, line)) { /* Do nothing, just skip the line */ }

    while (getline(in, line)) {
        if (line.find("Name: " + searchName) != string::npos) {
            found = true;
            name = searchName;
            for (int i = 0; i < 5 && getline(in, line); ++i) {
                if (i == 1) { age = stoi(line.substr(5)); }
                if (i == 2) { mobileNo = stoi(line.substr(15)); }
                if (i == 3) { address = line.substr(10); }
                if (i == 4) { gender = line.substr(9); }
            }
        }
    }
    in.close();
    return found;
}


class Cab {
    float pricePerKm;
    string type;

public:
    Cab() : pricePerKm(0), type("") {}  // Default constructor
    Cab(float price, const string& t) : pricePerKm(price), type(t) {}
    float computeCost(int km) const { return km * pricePerKm; }
    string getType() const { return type; }
};

class Booking {
    Customer* customer;
    Cab* cab;
    int kilometers;
    string hotel;
    float hotelCost;

public:
    Booking() : customer(nullptr), cab(nullptr) {}  // Default constructor
    Booking(Customer* c, Cab* cb, int km, const string& h, float hc)
        : customer(c), cab(cb), kilometers(km), hotel(h), hotelCost(hc) {}

    float getTotalCost() const;
    void printBill() const;
    void saveBooking(const string& customerName) const;
};

float Booking::getTotalCost() const {
    return hotelCost + cab->computeCost(kilometers);
}

void Booking::printBill() const {
    ofstream out("receipt.txt");
    out << "----------Air Travel Agency---------" << endl;
    out << "--------------Receipt---------------" << endl;
    out << "____________________________________" << endl;
    if (customer) {
        out << "Customer Name: " << customer->getName() << endl;
    }
    out << "Description\t\t Total" << endl;
    out << "Hotel cost:\t\t" << fixed << setprecision(2) << hotelCost << endl;
    out << "Travel (cab) cost:\t " << fixed << setprecision(2) << cab->computeCost(kilometers) << endl;
    out << "______________________________" << endl;
    out << "Total Charge:\t\t" << fixed << setprecision(2) << getTotalCost() << endl;
    out << "__________________________" << endl;
    out << "-----------THANK YOU-------" << endl;
    out.close();
}

void saveBooking(const string& customerName, Cab* selectedCab, int km, const string& hotel, float hotelCost) {
    ofstream out("bookings.txt", ios::app);
    out << "\nName: " << customerName << endl;
    out << "Cab Type: " << selectedCab->getType() << endl
        << "Kilometers: " << km << endl
        << "Hotel: " << hotel << endl
        << "Hotel Cost: " << hotelCost << endl;
    out.close();
}

void bookForCustomer(const string& customerName) {
    cout << "Booking for customer: " << customerName << endl;

    cout << "Enter Cab Type (Standard=1, Luxury=2): ";
    int cabType;
    cin >> cabType;
    Cab* selectedCab;
    if (cabType == 1) {
        selectedCab = new Cab(15, "Standard");
    } else {
        selectedCab = new Cab(25, "Luxury");
    }

    cout << "Enter kilometers: ";
    int km;
    cin >> km;

    string hotel;
    cout << "Enter Hotel Name: ";
    cin.ignore();
    getline(cin, hotel);

    cout << "Enter Hotel Cost: ";
    float hotelCost;
    cin >> hotelCost;

    Customer c;
    if (c.fetchCustomer(customerName)) {
        Booking b(&c, selectedCab, km, hotel, hotelCost);
        b.printBill();
        saveBooking(customerName, selectedCab, km, hotel, hotelCost);
        cout << "Booking successful!\n";
    } else {
        cout << "Customer not found! Please ensure the name is correct.\n";
    }

    delete selectedCab;  // Deallocate memory
}

int main() {
    while (true) {
        int choice;
        cout << "-------Air Travels Admin Menu------\n";
        cout << "1. Enter New Customer\n";
        cout << "2. See Old Customers\n";
        cout << "3. Book for Customer\n";
        cout << "4. Generate Bill\n";
        cout << "5. Exit\n";
        cout << "Enter Your Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                Customer newCustomer;
                newCustomer.getDetails();
                break;
            }
            case 2: {
                Customer cust;
                cust.showDetails();
                break;
            }
            case 3: {
                cout << "Enter Customer Name for Booking: ";
                string cname;
                cin.ignore();
                getline(cin, cname);
                thread bookingThread(bookForCustomer, cname);
                bookingThread.join();
                break;
            }
            case 4: {
                cout << "Enter the name of the customer for whom you want to generate a bill: ";
                string custName;
                cin.ignore();
                getline(cin, custName);

                // Fetch customer details
                Customer* fetchedCust = new Customer();
                
                // Open bookings file and search for customer booking details
                ifstream in("bookings.txt");
                if (!in) {
                    cout << "Unable to open bookings.txt" << endl;
                    delete fetchedCust;
                    break;
                }

                string line;
                Cab* bookedCab = nullptr;
                int bookedKm;
                string bookedHotel;
                float bookedHotelCost;

                bool n=false;

                while (getline(in, line)) {
                    

                    if (line == "Name: " + custName) {
                        n=true;
                        getline(in, line);  // Read Cab Type line
                        string cabType = line.substr(9); // Extract cab type
                        if (cabType == "Standard") {
                            bookedCab = new Cab(15, "Standard");
                        } else {
                            bookedCab = new Cab(25, "Luxury");
                        }

                        getline(in, line);  // Read Kilometers line
                        bookedKm = stoi(line.substr(12));

                        getline(in, line);  // Read Hotel line
                        bookedHotel = line.substr(7);

                        getline(in, line);  // Read Hotel Cost line
                        bookedHotelCost = stof(line.substr(12));

                        // Create the booking object using fetched details
                        Booking booking(fetchedCust, bookedCab, bookedKm, bookedHotel, bookedHotelCost);
                        booking.printBill();
                        cout << "Bill generated successfully!" << endl;

                        delete bookedCab; // Cleanup
                        break;

                        
                    }
                }
                if(n==false){

                    cout << "Customer not found. Cannot generate bill." << endl;

                }

                in.close();
                delete fetchedCust;  // Cleanup
                break;
            }

            case 5:
                cout << "Exiting... Goodbye!\n";
                return 0;
            default:
                cout << "Invalid Choice. Please choose again.\n";
                break;
        }
    }

    return 0;
}
