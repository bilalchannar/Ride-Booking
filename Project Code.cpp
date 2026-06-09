#include "RideBookingEngine.h"

// --- Global variables are now in RideBookingEngine.cpp ---

void simulateLoading(string msg) {
    cout << CYAN << msg;
    for (int i = 0; i < 3; i++) {
        this_thread::sleep_for(chrono::milliseconds(400));
        cout << ".";
    }
    cout << RESET << endl;
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void printHeader(string title) {
    cout << CYAN << "==========================================================" << RESET << endl;
    cout << BOLD << YELLOW << "                " << title << RESET << endl;
    cout << CYAN << "==========================================================" << RESET << endl;
}

void printLogo() {
    cout << CYAN << R"(
    ____  _     __        ____              __   _            
   / __ \(_)___/ /__     / __ )____  ____  / /__(_)___  ____ _
  / /_/ / / __  / _ \   / __  / __ \/ __ \/ //_/ / __ \/ __ `/
 / _, _/ / /_/ /  __/  / /_/ / /_/ / /_/ / ,< / / / / / /_/ / 
/_/ |_/_/\__,_/\___/  /_____/\____/\____/_/|_/_/_/ /_/\__, /  
                                                      /____/   
    )" << RESET << endl;
    cout << YELLOW << "          --- Premium Ride Booking System v2.0 ---" << RESET << endl << endl;
}

void drawMiniMap() {
    cout << BOLD << "       [City Map Visualization]" << RESET << endl;
    cout << "       F6 --- F7 --- F8 --- F9 --- PAF" << endl;
    cout << "       |      |      |      | " << endl;
    cout << "     Blue --- G7 --- G8 --- G9 --- G10 --- G11" << endl;
    cout << "       |      |      |      |               | " << endl;
    cout << "      G6 --- Zero -- I8 --- I9 --- I10 --- FAST" << endl;
    cout << "              |      |              |       | " << endl;
    cout << "              I9 --- Faiz --- Stad --- NUST" << endl << endl;
}

int readInt(string prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        } else {
            cout << RED << "Invalid input. Please enter a valid number." << RESET << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

double readDouble(string prompt) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        } else {
            cout << RED << "Invalid input. Please enter a valid numeric value." << RESET << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

string getPasswordInput(string prompt) {
    string password = "";
    char ch;
    cout << prompt;
    while ((ch = _getch()) != '\r') {
        if (ch == '\b') {
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b";
            }
        } else {
            password += ch;
            cout << '*';
        }
    }
    cout << endl;
    return password;
}

void listAllLocations() {
    cout << "\n--- All Locations in the City Graph ---" << endl;
    cityZones.display();
}

void handle_SignUp() {
    string n, e, p, pass;
    printHeader("USER REGISTRATION");
    cout << "Name: "; getline(cin, n);
    cout << "Email: "; getline(cin, e);
    cout << "Phone: "; getline(cin, p);
    pass = getPasswordInput("Create Password: ");
    userMap[toLowerCase(e)] = User(toLowerCase(e), n, p, hashPassword(pass));
    simulateLoading("Creating Account");
    cout << GREEN << "[✔] Account created! Please log in." << RESET << endl;
    saveData();
    this_thread::sleep_for(chrono::seconds(1));
}

void handle_Login() {
    string emailInput, passInput;
    printHeader("USER LOGIN");
    cout << "Enter your email: "; getline(cin, emailInput);
    string lowerEmail = toLowerCase(emailInput);
    if (userMap.count(lowerEmail)) {
        passInput = getPasswordInput("Enter your password: ");
        simulateLoading("Authenticating");
        if (userMap[lowerEmail].password == hashPassword(passInput)) {
            currentLoggedInUserEmail = lowerEmail;
            cout << GREEN << "\n[✔] Access Granted! Welcome, " << userMap[currentLoggedInUserEmail].name << "!" << RESET << endl;
            this_thread::sleep_for(chrono::seconds(1));
        } else {
            cout << RED << "\n[✘] Incorrect password!" << RESET << endl;
            this_thread::sleep_for(chrono::seconds(1));
        }
    } else {
        cout << RED << "\n[✘] Email not found." << RESET << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }
}

void handle_Admin() {
    string pass;
    printHeader("ADMIN ACCESS");
    pass = getPasswordInput("Enter Admin Password: ");
    if (pass == "admin123") {
        bool adminRunning = true;
        while (adminRunning) {
            clearScreen();
            printHeader("ADMIN CONTROL PANEL");
            cout << "1. View All Users\n2. View All Drivers\n3. Add New Driver\n4. Route Analytics\n5. Exit Admin Panel\n";
            int choice = readInt("Choice: ");
            switch (choice) {
                case 1:
                    clearScreen(); printHeader("REGISTERED USERS");
                    for (auto const &entry : userMap) {
                        cout << "👤 " << entry.second.name << " | " << entry.second.email << " | Wallet: Rs. " << entry.second.balance << endl;
                    }
                    _getch(); break;
                case 2:
                    clearScreen(); printHeader("ACTIVE DRIVERS");
                    for (auto const &entry : driverInfoMap) {
                        cout << "🚖 " << entry.second.name << " | Loc: " << entry.second.location << " | Rating: " << entry.second.rating << endl;
                    }
                    _getch(); break;
                case 3: {
                    clearScreen(); printHeader("ADD NEW DRIVER");
                    string n, l, m, p, t;
                    cout << "Name: "; getline(cin, n);
                    cout << "Location: "; getline(cin, l);
                    cout << "Car Model: "; getline(cin, m);
                    cout << "Plate No: "; getline(cin, p);
                    cout << "Type: "; getline(cin, t);
                    driverInfoMap[n] = Driver(n, l, 4.0);
                    driverVehicleMap[n] = Vehicle {p, n, m, t};
                    saveData(); break;
                }
                case 4:
                    clearScreen(); printHeader("ROUTE ANALYTICS");
                    for (auto const &pair : popularRoutes) cout << "📍 " << pair.first << " : " << pair.second << " rides" << endl;
                    _getch(); break;
                case 5: adminRunning = false; break;
            }
        }
    } else {
        cout << RED << "\n[✘] Access Denied." << RESET << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }
}

int main() {
    buildGraph();
    loadData();

    bool running = true;
    while (running) {
        clearScreen();
        printLogo();
        if (currentLoggedInUserEmail.empty()) {
            printHeader("GET STARTED");
            cout << "1. Login\n2. Sign Up\n3. Exit\n";
            int startChoice = readInt("Select Option: ");
            if (startChoice == 1) handle_Login();
            else if (startChoice == 2) handle_SignUp();
            else if (startChoice == 3) running = false;
            continue;
        }

        User &currentUser = userMap[currentLoggedInUserEmail];
        printHeader("MAIN MENU");
        cout << "👤 User: " << CYAN << currentUser.name << RESET << " | 💰 Wallet: " << GREEN << "Rs. " << currentUser.balance << RESET << " | ✨ Points: " << YELLOW << currentUser.loyaltyPoints << RESET << endl << endl;
        cout << "1. Book Ride\n2. History\n3. Recharge\n4. Map\n5. Admin\n6. Log Out\n7. Exit\n";
        int choice = readInt("Choice: ");

        switch (choice) {
            case 1: {
                clearScreen(); printHeader("BOOK A RIDE"); drawMiniMap();
                string p, d;
                cout << "Pickup: "; getline(cin, p);
                cout << "Drop: "; getline(cin, d);
                string normP = normalize(p), normD = normalize(d);
                if (cityZones.search(normP) && cityZones.search(normD)) {
                    if (nameMap[normP] == nameMap[normD]) {
                        cout << RED << "Pickup and Drop-off locations cannot be the same!" << RESET << endl;
                    } else {
                        pair<vector<string>, int> res = dijkstra(nameMap[normP], nameMap[normD], "distance");
                        if (res.second != -1) {
                            cout << "Shortest Path: ";
                            for (int i=0; i<res.first.size(); i++) cout << res.first[i] << (i==res.first.size()-1?"":" -> ");
                            cout << "\nDistance: " << res.second << " km\n";

                            // 1. Select Ride Class
                            string preferredRideType;
                            bool validRideType = false;
                            while (!validRideType) {
                                cout << "\nWhich ride type do you prefer?\n";
                                cout << "1. Bike\n";
                                cout << "2. Mini (Compact Car)\n";
                                cout << "3. Business Class (Luxury Car)\n";
                                int typeChoice = readInt("Enter choice (1-3): ");
                                switch (typeChoice) {
                                    case 1: preferredRideType = "Bike"; validRideType = true; break;
                                    case 2: preferredRideType = "Mini"; validRideType = true; break;
                                    case 3: preferredRideType = "Business Class"; validRideType = true; break;
                                    default: cout << RED << "Invalid choice. Please enter 1, 2, or 3." << RESET << endl; break;
                                }
                            }
                            cout << "\nYou selected: " << preferredRideType << ".\n";

                            // 2. Calculate dynamic fare
                            int suggestedFare;
                            if (preferredRideType == "Bike") suggestedFare = res.second * 15;
                            else if (preferredRideType == "Mini") suggestedFare = res.second * 20;
                            else suggestedFare = res.second * 35;

                            suggestedFare = static_cast<int>(suggestedFare * surgeMultiplier);
                            cout << "Suggested Approximate Fare: Rs. " << suggestedFare << endl;

                            // 3. Fare negotiation
                            int userOfferedFare;
                            const double TOLERANCE = 0.05;
                            bool fareAccepted = false;
                            while (!fareAccepted) {
                                userOfferedFare = readInt("Enter the maximum fare you are willing to pay (Rs.): ");
                                if (userOfferedFare >= suggestedFare * (1.0 - TOLERANCE) &&
                                    userOfferedFare <= suggestedFare * (1.0 + TOLERANCE)) {
                                    fareAccepted = true;
                                    cout << GREEN << "Your offered fare (Rs. " << userOfferedFare << ") is acceptable." << RESET << endl;
                                } else {
                                    cout << RED << "Your offered fare is not within the acceptable range (Rs. "
                                         << static_cast<int>(suggestedFare * (1.0 - TOLERANCE)) << " - " 
                                         << static_cast<int>(suggestedFare * (1.0 + TOLERANCE)) << ")." << RESET << endl;
                                }
                            }

                            // 4. Find drivers
                            vector<AvailableDriver> potentialDrivers;
                            for (const auto &pair : driverInfoMap) {
                                Driver driver = pair.second;
                                if (driverVehicleMap.count(driver.name)) {
                                    Vehicle vehicle = driverVehicleMap[driver.name];
                                    if (vehicle.carType == preferredRideType) {
                                        auto d_res = dijkstra(driver.location, nameMap[normP], "distance");
                                        if (d_res.second != -1) {
                                            potentialDrivers.push_back(AvailableDriver(driver.name, d_res.second, driver.rating));
                                        }
                                    }
                                }
                            }

                            if (potentialDrivers.empty()) {
                                cout << RED << "\nNo suitable drivers found for ride type (" << preferredRideType << "). Try again later." << RESET << endl;
                                this_thread::sleep_for(chrono::seconds(2));
                                break;
                            }

                            sort(potentialDrivers.begin(), potentialDrivers.end(), [](const AvailableDriver &a, const AvailableDriver &b) {
                                if (a.distance != b.distance) return a.distance < b.distance;
                                return a.rating > b.rating;
                            });

                            // 5. Driver Selection Loop
                            string assignedDriverName = "";
                            bool driverAccepted = false;
                            size_t currentDriverIndex = 0;

                            while (!driverAccepted && currentDriverIndex < potentialDrivers.size()) {
                                AvailableDriver d = potentialDrivers[currentDriverIndex];
                                cout << "\nDriver: " << CYAN << d.name << RESET << " | Proximity: " << d.distance << " km | Rating: " << YELLOW << d.rating << RESET << endl;
                                if (driverVehicleMap.count(d.name)) {
                                    Vehicle v = driverVehicleMap[d.name];
                                    cout << "Vehicle: " << v.carType << " - " << v.carModel << " (Plate: " << v.noPlate << ")\n";
                                }
                                cout << "Offered Fare: Rs. " << userOfferedFare << endl;
                                cout << "Accept this driver (y/n)? (q to quit): ";
                                char choice; cin >> choice;
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                                if (choice == 'y' || choice == 'Y') {
                                    assignedDriverName = d.name;
                                    driverAccepted = true;
                                } else if (choice == 'n' || choice == 'N') {
                                    currentDriverIndex++;
                                } else if (choice == 'q' || choice == 'Q') {
                                    cout << YELLOW << "Ride booking cancelled." << RESET << endl;
                                    break;
                                } else {
                                    cout << RED << "Invalid input." << RESET << endl;
                                }
                            }

                            // 6. Confirm Booking
                            if (driverAccepted) {
                                cout << "\nConfirm booking with " << assignedDriverName << " (y/n)? ";
                                char finalConfirm; cin >> finalConfirm;
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                                if (finalConfirm == 'y' || finalConfirm == 'Y') {
                                    if (currentUser.balance >= userOfferedFare) {
                                        simulateLoading("Ride in progress");
                                        currentUser.balance -= userOfferedFare;
                                        currentUser.loyaltyPoints += userOfferedFare / 10;
                                        popularRoutes[nameMap[normP] + " to " + nameMap[normD]]++;
                                        
                                        // Move driver to drop location
                                        driverInfoMap[assignedDriverName].location = nameMap[normD];

                                        currentUser.rideHistory.addRide(Ride(currentUser.email, assignedDriverName, nameMap[normP], nameMap[normD], userOfferedFare, res.second, preferredRideType));
                                        cout << GREEN << "Ride Completed! Thank you for choosing us." << RESET << endl;
                                        saveData();
                                    } else {
                                        cout << RED << "Insufficient balance in wallet!" << RESET << endl;
                                    }
                                } else {
                                    cout << YELLOW << "Ride cancelled." << RESET << endl;
                                }
                            }
                        } else cout << RED << "No route found!" << RESET << endl;
                    }
                } else cout << RED << "Invalid Locations!" << RESET << endl;
                this_thread::sleep_for(chrono::seconds(2)); break;
            }
            case 2: clearScreen(); printHeader("RIDE HISTORY"); currentUser.rideHistory.displayHistory(); _getch(); break;
            case 3: {
                double amt = readDouble("Amount: ");
                currentUser.balance += amt;
                saveData(); break;
            }
            case 4: clearScreen(); listAllLocations(); _getch(); break;
            case 5: handle_Admin(); break;
            case 6: currentLoggedInUserEmail = ""; break;
            case 7: running = false; break;
        }
    }
    return 0;
}
