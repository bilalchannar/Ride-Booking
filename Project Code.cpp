#include <iostream>
#include <unordered_map>
#include <limits>
#include <cctype> 
#include <vector> 
#include <algorithm>     
#include <queue>

using namespace std;

struct Edge {
	string destination; 
	int distance;
	int time;

	Edge(string dest, int dis, int ti) {
		destination = dest, distance = dis, time = ti;
	}
};

struct Driver {
	string name;
	string location;
	double rating;

	Driver() {}
	Driver(string n, string l, double r) : name(n), location(l), rating(r) {} 
};

struct Ride { //Stores the full ride details 
    string userEmail; 
    string driverID;
    string from;
    string to;
    int fare;          
    int distance;
    string rideType; 

    Ride() : userEmail(""), driverID(""), from(""), to(""), fare(0), distance(0), rideType("") {}

    Ride(string u_email, string d, string f, string t, int fa, int dis, string r_type)
        : userEmail(u_email), driverID(d), from(f), to(t), fare(fa), distance(dis), rideType(r_type) {}
};
struct RideNode {
    Ride ride;
    RideNode* next;

    RideNode(Ride r) : ride(r), next(nullptr) {}
};

class RideHistory {
private:
    vector<Ride> rides;

public:
    void addRide(const Ride& r) {
        rides.push_back(r);
    }

    void displayHistory() {
        if (rides.empty()) {
            cout << "No ride history to display.\n";
            return;
        }
        for (Ride& r : rides) {
            cout << "From " << r.from << " to " << r.to<< " | Fare: Rs. " << r.fare << " | Distance: " << r.distance << " km | Type: " << r.rideType << endl;
        }
    }

    vector<Ride>& getRides() {
        return rides;
    }
};
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct User {
    string email;
    string name;
    string phone;
    RideHistory rideHistory;

    User() {}

    User(string e, string n,string p)
        : email(e), name(n), phone(p), rideHistory() {}
};

struct Vehicle {
	string noPlate;
	string driverName;
	string carModel;
	string carType;
};
////Before Ride Starts
struct RideRequest { 
	string userEmail;
	string userName;
	string pickup;
	string drop;
	string rideType;

	RideRequest(string u_email, string u_name, string p, string d, string r_type) {
		userEmail = u_email, userName = u_name,pickup = p, drop = d, rideType = r_type;
	}
};
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct AvailableDriver { // type of data we are storing 
    string name;       
    double distance;   
    double rating;     

    AvailableDriver(string n, double d_param, double r_param)
        : name(n), distance(d_param), rating(r_param) {}
};

struct CompareAvailableDriver {
    bool operator()(const AvailableDriver& d1, const AvailableDriver& d2) { // not medifying the variables
        if (d1.distance == d2.distance) {
            return d1.rating > d2.rating; //min heap
        }
        return d1.distance < d2.distance;
    }
};
class DriverPriorityQueue {
private:
	
    priority_queue<AvailableDriver, vector<AvailableDriver>, CompareAvailableDriver> pq; //vector is unerlying container to store element, or then it is comapred with CAD

public:
    void addDriver(const AvailableDriver& d) {
        pq.push(d);
    }
    AvailableDriver getBestDriver() {
        if (pq.empty()) {
            throw runtime_error("No drivers available in priority queue.");
        }
        AvailableDriver best = pq.top();
        pq.pop();
        return best;
    }

    bool empty() const  { 
        return pq.empty();
    }
    
    void displayDrivers() { 
        priority_queue<AvailableDriver, vector<AvailableDriver>, CompareAvailableDriver> temp_pq = pq;
        if (temp_pq.empty()) {
            cout << "No available drivers in the queue." << endl;
            return;
        }
        cout << "--- Available Drivers in Priority Queue---" << endl;
        while(!temp_pq.empty()) {
            AvailableDriver d = temp_pq.top();
            temp_pq.pop();
            cout << "- Driver: " << d.name << ", Distance: " << d.distance << "km, Rating: " << d.rating << endl;
        }
    }
};
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct ZoneNode {
	string zoneName;
	ZoneNode* left;
	ZoneNode* right;\
	\
	\
	\
	
	\
	

	ZoneNode(string name) {
		zoneName = name, left = nullptr, right = nullptr;
	}
};

class ZoneBST {
	private:
		ZoneNode* root;

		ZoneNode* insertRec(ZoneNode* node, string zone) {
			if (!node) return new ZoneNode(zone);
			if (zone < node->zoneName) 
				node->left = insertRec(node->left, zone);
			else if (zone > node->zoneName)
				node->right = insertRec(node->right, zone);
			return node;
		}

		bool searchRec(ZoneNode* node, string zone) {
			if (!node) return false;
			if (zone == node->zoneName) return true;
			if (zone < node->zoneName) return searchRec(node->left, zone);
			else return searchRec(node->right, zone);
		}

		void inorderRec(ZoneNode* node) {
			if (!node) return;
			inorderRec(node->left);
			cout << node->zoneName << endl;
			inorderRec(node->right);
		}

		void deleteTree(ZoneNode* node) {
			if (!node) return;
			deleteTree(node->left);
			deleteTree(node->right);
			delete node;
		}

	public:
		ZoneBST() : root(nullptr) {}
		~ZoneBST() {
			deleteTree(root);
		}

		void insert(string zone) {
			root = insertRec(root, zone);
		}

		bool search(string zone) {
			return searchRec(root, zone);
		}

		ZoneNode* getRoot() {
			return root;
		}

		void displayZones() {
			if (!root) {
				cout << "No zones to display." << endl;
				return;
			}
			cout << "--- City Zones (sorted alphabetically) ---" << endl;
			inorderRec(root);
		}
};

unordered_map<string, User> userMap; 
unordered_map<string, string>nameMap;
unordered_map<string, vector<Edge>> graph; 
unordered_map<string, Driver> driverInfoMap; 
unordered_map<string, Vehicle> driverVehicleMap;

int nextRideID = 1;      
ZoneBST cityZones; 

string currentLoggedInUserEmail = "";

string toLowerCase(string str) {
	string lower = str;
	transform(lower.begin(), lower.end(), lower.begin(), ::tolower); // algorithm.. Converts a single character to its lowercase form.
	return lower;
}

void buildGraph() {
	graph["F6"] = { Edge("F7", 2, 4), Edge("Blue Area", 3, 6), Edge("QAU", 6, 12) };
	graph["F7"] = { Edge("F6", 2, 4), Edge("F8", 2, 5), Edge("Blue Area", 2, 4) };
	graph["F8"] = { Edge("F7", 2, 5), Edge("F9", 2, 4), Edge("Faisal Mosque", 3, 7), Edge("G8", 3, 6) };
	graph["F9"] = { Edge("F8", 2, 4), Edge("F10", 3, 5), Edge("Air Uni", 3, 6), Edge("G9", 2, 4) };
	graph["F10"] = { Edge("F9", 3, 5), Edge("G9", 3, 5), Edge("PAF Hospital", 1, 2), Edge("G10", 2, 4), Edge("G11", 3, 6) };
	graph["G6"] = { Edge("G7", 2, 4), Edge("Shakarparian", 3, 5), Edge("Bani Gala", 4, 8), Edge("Blue Area", 2, 4) };
	graph["G7"] = { Edge("G6", 2, 4), Edge("G8", 2, 4), Edge("Blue Area", 2, 4), Edge("Zero Point", 3, 6) };
	graph["G8"] = { Edge("G7", 2, 4), Edge("G9", 2, 4), Edge("Zero Point", 2, 5), Edge("F8", 3, 6) };
	graph["G9"] = { Edge("G8", 2, 4), Edge("G10", 2, 4), Edge("F10", 3, 5), Edge("F9", 2, 4) };
	graph["G10"] = { Edge("G9", 2, 4), Edge("G11", 2, 4), Edge("F10", 2, 4) };
	graph["G11"] = { Edge("G10", 2, 4), Edge("FAST", 3, 6), Edge("NUST", 4, 8), Edge("F10", 3, 6) };
	graph["Blue Area"] = { Edge("F7", 2, 4), Edge("G7", 2, 4), Edge("G6", 2, 4), Edge("F6", 3, 6) };
	graph["Zero Point"] = { Edge("G8", 2, 4), Edge("I8", 3, 5), Edge("G7", 2, 4) };
	graph["Shakarparian"] = { Edge("G6", 3, 6), Edge("Comsats", 5, 10), Edge("G7", 2, 4) };
	graph["Bani Gala"] = { Edge("G6", 4, 8), Edge("QAU", 6, 12), Edge("Comsats", 4, 8) };
	graph["QAU"] = { Edge("F6", 6, 12), Edge("Bani Gala", 6, 12) };
	graph["I8"] = { Edge("Zero Point", 3, 5), Edge("I9", 2, 4), Edge("Faizabad", 4, 7), Edge("Comsats", 3, 6) };
	graph["I9"] = { Edge("I8", 2, 4), Edge("I10", 2, 4) };
	graph["I10"] = { Edge("I9", 2, 4), Edge("I11", 3, 5), Edge("FAST", 2, 4), Edge("Pirwadhai", 4, 8) };
	graph["I11"] = { Edge("I10", 3, 5), Edge("I12", 3, 6) };
	graph["I12"] = { Edge("I10", 3, 5), Edge("NUST", 2, 4) };
	graph["Pirwadhai"] = { Edge("I10", 4, 8), Edge("Saddar", 5, 10) };
	graph["Faizabad"] = { Edge("I8", 4, 7), Edge("Stadium", 2, 4), Edge("Saddar", 6, 12) };
	graph["Stadium"] = { Edge("Faizabad", 2, 4), Edge("Saddar", 3, 5) };
	graph["Saddar"] = { Edge("Pirwadhai", 5, 10), Edge("Stadium", 3, 5), Edge("Faizabad", 6, 12) };
	graph["PAF Hospital"] = { Edge("F10", 1, 2), Edge("F9", 2, 4) };
	graph["Air Uni"] = { Edge("F9", 3, 6), Edge("F8", 3, 6) };
	graph["Faisal Mosque"] = { Edge("F8", 3, 6) };
	graph["FAST"] = { Edge("G11", 3, 6), Edge("I10", 2, 4) };
	graph["NUST"] = { Edge("G11", 4, 8), Edge("I12", 2, 4) };
	graph["Comsats"] = { Edge("Shakarparian", 5, 10), Edge("Bani Gala", 4, 8), Edge("I8", 3, 6) };

	for (const auto& pair : graph) {
		nameMap[toLowerCase(pair.first)] = pair.first;
		cityZones.insert(toLowerCase(pair.first));
	}
}

struct ComparePair {
    bool operator()(const pair<string, int>& a, const pair<string, int>& b) {
        return a.second > b.second; 
    }
};

pair<vector<string>, int> dijkstra(const string& start, const string& end, const string& mode) { // key id string
    if (graph.find(start) == graph.end() || graph.find(end) == graph.end()) {
        cerr << "Error: Start or end location not found.\n";
        return {{}, -1};
    }

    unordered_map<string, int> dist; 
    unordered_map<string, string> parent; 

    priority_queue<pair<string, int>, vector<pair<string, int>>, ComparePair> pq; 

    for (auto& [node, _] : graph) { 
        dist[node] = INT_MAX; 
        parent[node] = "";
    }

    dist[start] = 0;
    pq.push({start, 0});

    while (!pq.empty()) {
        auto [current, currDist] = pq.top(); 
        pq.pop();

        if (currDist > dist[current]) continue;
        if (current == end) break;

        for (auto& edge : graph[current]) {
            int weight = (mode == "time") ? edge.time : edge.distance;
            if (dist[current] + weight < dist[edge.destination]) {  // Fixed: edge.to -> edge.destination
                dist[edge.destination] = dist[current] + weight;
                parent[edge.destination] = current;
                pq.push({edge.destination, dist[edge.destination]});
            }
        }
    }

    vector<string> path;
    if (dist[end] == INT_MAX) return {path, -1}; //If the shortest path is still ∞, that means no way to reach the destination.

    for (string at = end; at != ""; at = parent[at]) { // if the end point is the start point 
        path.push_back(at);
        if (at == start) break;
    }

    reverse(path.begin(), path.end());
    return {path, dist[end]};
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void listAllLocations() {
	cout << "\n--- All Locations in the City Graph ---\n";
	if (cityZones.getRoot() == nullptr) {
		cout << "Graph is empty. No locations to display." << endl;
		return;
	}
	cityZones.displayZones();
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void handle_SignUp() {
	string user_Name, user_Email, user_Phone;
	cout << "\n--- New User Sign Up ---\n";
	cout << "Enter your Full Name: ";
	getline(cin, user_Name);

	bool validPhone = false;
	while (!validPhone) {
		cout << "Enter your Phone Number (11 digits, e.g., 03XXXXXXXXX): ";
		getline(cin, user_Phone);
		if (user_Phone.length() == 11) {
			bool allDigits = true;
			for (char c : user_Phone) {
				if (!isdigit(c)) {
					allDigits = false;
					break;
				}
			}
			if (allDigits) {
				validPhone = true;
			} else {
				cout << "Invalid phone number. Please enter only digits.\n";
			}
		} else {
			cout << "Invalid phone number. Please enter exactly 11 digits.\n";
		}
	}

	bool validEmail = false;
	while (!validEmail) {
		cout << "Enter your Email (must end with @gmail.com): ";
		getline(cin, user_Email);
		string lowerEmail = toLowerCase(user_Email);
		if (lowerEmail.length() >= 10 && lowerEmail.substr(lowerEmail.length() - 10) == "@gmail.com") { //returns a substring of the string
			validEmail = true;
		} else {
			cout << "Invalid email address. Please use an email ending with @gmail.com.\n";
		}
	}

	string lowerEmail = toLowerCase(user_Email);
	if (userMap.count(lowerEmail)) {       //Checks how many times lowerEmail appears as a key in the map.
		cout << "Error: An account with this email already exists. Please try logging in instead.\n";
	} else {
		userMap[lowerEmail] = User(lowerEmail, user_Name, user_Phone);
		cout << "\nAccount created successfully for " << user_Name << "!\n";
		currentLoggedInUserEmail = lowerEmail;
		cout << "\n You are now logged in.\n";
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void handle_Login() {
	string emailInput;
	cout << "\n--- User Log In ---\n";
	cout << "Enter your email: ";
	getline(cin, emailInput);
	string lowerEmail = toLowerCase(emailInput);

	if (userMap.count(lowerEmail)) {
		currentLoggedInUserEmail = lowerEmail;
		cout << "Successfully logged in as " << userMap[currentLoggedInUserEmail].name << "!\n";
	} else {
		cout << "Email not found. Please ensure you have signed up first, or try signing up now.\n";
	}
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
int main() {
	buildGraph();  
	
	userMap["ali@gmail.com"] = User("ali@gmail.com", "Ali", "12345678910");
	  
 	driverInfoMap["Ali Driver"] = Driver{"Ali Driver", "F6", 4};
    driverVehicleMap["Ali Driver"] = Vehicle{"ABC123", "Ali Driver", "Toyota Corolla", "Mini"}; // Set type

    driverInfoMap["Faiza Driver"] = Driver{"Faiza Driver", "G7", 5};
    driverVehicleMap["Faiza Driver"] = Vehicle{"XYZ789", "Faiza Driver", "Honda Civic", "Mini"}; // Set type

    driverInfoMap["John Driver"] = Driver{"John Driver", "I8", 3};
    driverVehicleMap["John Driver"] = Vehicle{"DEF456", "John Driver", "Suzuki Alto", "Mini"}; // Set type

    driverInfoMap["Bilal Rider"] = Driver{"Bilal Rider", "F7", 4};
	driverVehicleMap["Bilal Rider"] = Vehicle{"BIKE001", "Bilal Rider", "Honda 125", "Bike"};

    driverInfoMap["Ahad Driver"] = Driver{"Ahad Driver", "G6", 5};
    driverVehicleMap["Ahad Driver"] = Vehicle{"LUX777", "Ahad Driver", "Mercedes C-Class", "Business Class"};

	bool running = true;
	while (running) {
		if (currentLoggedInUserEmail.empty()) {
			char registeredChoice;
			cout << "\nAre you a registered user (y/n)? ";
			cin >> registeredChoice;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			if (registeredChoice == 'y' || registeredChoice == 'Y') {
				handle_Login();
			} else if (registeredChoice == 'n' || registeredChoice == 'N') {
				handle_SignUp();
			} else {
				cout << "Invalid input. Please enter 'y' or 'n'.\n";
			}
			if (currentLoggedInUserEmail.empty()) {
				continue;
			}
		}

		cout << "\n======== Welcome, " << userMap[currentLoggedInUserEmail].name << " to the ride booking app ========\n";
		cout << "1. Book a Ride\n";
		cout << "2. View Ride History\n";
		cout << "3. Show All Locations\n";
		cout << "4. Log Out\n";
		cout << "5. Exit\n"; 
		cout << "Enter choice: ";

		int menuChoice;
		cin >> menuChoice;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

switch (menuChoice) {
	case 1: {
		try {
			User& currentUser = userMap[currentLoggedInUserEmail]; // login email
			string pickupInput, dropInput;
			string originalPickup, originalDrop;
			bool validLocations = false;
			
			while (!validLocations) {
				cout << "Enter Pickup Location: ";
				getline(cin, pickupInput);
				cout << "Enter Drop-off Location: ";
				getline(cin, dropInput);

				string lowerPickup = toLowerCase(pickupInput);
				string lowerDrop = toLowerCase(dropInput);
				
				if (!cityZones.search(lowerPickup)) {
					cout << "Error: Pickup location '" << pickupInput << "' is not a valid city zone. Please try again." << endl;
				} else if (!cityZones.search(lowerDrop)) {
					cout<< "Error: Drop-off location '" << dropInput << "' is not a valid city zone. Please try again." << endl;
				} else {
					originalPickup = nameMap[lowerPickup];   // Get original casing from nameMap for Dijkstra and storage
					originalDrop = nameMap[lowerDrop];

					if (originalPickup == originalDrop) {
						cout << "Error: Pickup and drop-off locations cannot be the same. Please try again." << endl;
					} else {
						validLocations = true;
					}
				}
			}
			  // Ask for preferred ride type
			string preferredRideType;
			bool validRideType = false;
			while (!validRideType) {
				cout << "\nWhich ride type do you prefer?\n";
				cout << "1. Bike\n";
				cout << "2. Mini (Compact Car)\n";
				cout << "3. Business Class (Luxury Car)\n";
				cout << "Enter choice (1-3): ";
				int typeChoice;
				cin >> typeChoice;
				cin.ignore(numeric_limits<streamsize>::max(), '\n');

				switch (typeChoice) {
					case 1: preferredRideType = "Bike"; validRideType = true; break;
					case 2: preferredRideType = "Mini"; validRideType = true; break;
					case 3: preferredRideType = "Business Class"; validRideType = true; break;
					default: cout << "Invalid choice. Please enter 1, 2, or 3.\n"; break;
				}
			}
			cout << "\nYou selected: " << preferredRideType << ".\n";

			RideRequest currentRequest(currentUser.email, currentUser.name, originalPickup, originalDrop, preferredRideType);
			cout << "\nProcessing request for " << currentRequest.userName << " from " << currentRequest.pickup << " to " << currentRequest.drop << " (" << currentRequest.rideType << ")\n";

			auto [path, ride_distance] = dijkstra(currentRequest.pickup, currentRequest.drop, "distance"); // acces the paths and 2nd one shows the distance 
			auto [time_path, ride_time] = dijkstra(currentRequest.pickup, currentRequest.drop, "time");

			if (ride_distance == -1) {
				cout << "\nNo route found from " << currentRequest.pickup << " to " << currentRequest.drop << endl;
				break;
			}

			cout << "\nShortest Path (Distance): ";
			for (int i = 0; i < path.size(); ++i) {
				cout << path[i] << "-> ";
			}
			cout << "\n\nTotal Distance: " << ride_distance << " km\n";
			cout << "Estimated Time: " << ride_time << " minutes\n";
 // --- DYNAMIC FARE CALCULATION BASED ON RIDE TYPE ----------------------------------------------------------------------------------------------------------------------
			int suggestedFare;
			if (preferredRideType == "Bike") suggestedFare = ride_distance * 15;
			else if (preferredRideType == "Mini") suggestedFare = ride_distance * 20;
			else suggestedFare = ride_distance * 35;

			cout << "\nSuggested Approximate Fare: Rs. " << suggestedFare << endl;

			int userOfferedFare;
			const double TOLERANCE = 0.05;
			bool fareAccepted = false;

			while (!fareAccepted) {
				cout << "Enter the maximum fare you are willing to pay (Rs.): ";
				cin >> userOfferedFare;
				cin.ignore(numeric_limits<streamsize>::max(), '\n');

				if (userOfferedFare >= suggestedFare * (1.0 - TOLERANCE) &&
					userOfferedFare <= suggestedFare * (1.0 + TOLERANCE)) {
					fareAccepted = true;
					cout << "Your offered fare (Rs. " << userOfferedFare << ") is acceptable.\n";
				} else {
					cout << "Your offered fare is not within the acceptable range (Rs. "
						 << static_cast<int>(suggestedFare * (1.0 - TOLERANCE)) << " - " 
						 << static_cast<int>(suggestedFare * (1.0 + TOLERANCE)) << ").\n";
				}
			}
 // --- Driver Assignment Logic (Iterative Selection - NOW FILTERS BY RIDE TYPE) ------------------------------------------------------------------------------------
			vector<AvailableDriver> potentialDrivers;
			for (const pair<string, Driver>& pair : driverInfoMap) { //Each element of the map is a pair, where the key is a string (e.g. email or name), and the value is a Driver object
				Driver driver = pair.second;
				if (driverVehicleMap.count(driver.name)) {
					Vehicle vehicle = driverVehicleMap[driver.name];
					if (vehicle.carType == preferredRideType) {
						auto [d_path, d_dist] = dijkstra(driver.location, currentRequest.pickup, "distance"); //d_path → the route (not used here) d_dist → the numerical distance between the driver and the rider
						if (d_dist != -1) {
							potentialDrivers.push_back(AvailableDriver(driver.name, d_dist, driver.rating));
						}
					}
				}
			}

			if (potentialDrivers.empty()) {
				cout << "\nNo suitable drivers found for ride type (" << preferredRideType << "). Try again later.\n";
				break;
			}
//---------------------------------------------------------------------------------------------------------------------------------------------------
			sort(potentialDrivers.begin(), potentialDrivers.end(), CompareAvailableDriver());

			string assignedDriverName = "";
			bool driverAccepted = false;
			int currentDriverIndex = 0;

			while (!driverAccepted && currentDriverIndex < potentialDrivers.size()) {
				AvailableDriver d = potentialDrivers[currentDriverIndex];
				cout << "\nDriver: " << d.name << " | Distance: " << d.distance << " km | Rating: " << d.rating << endl;

				if (driverVehicleMap.count(d.name)) {
					Vehicle v = driverVehicleMap[d.name];
					cout << "Vehicle: " << v.carType << " - " << v.carModel << " (Plate: " << v.noPlate << ")\n";
				} else {
					cout << "No vehicle info available.\n";
				}

				cout << "\nFare: Rs. " << userOfferedFare << "\n";
				cout << "Accept this driver (y/n)? (q to quit): ";
				char choice;
				cin >> choice;
				cin.ignore(numeric_limits<streamsize>::max(), '\n');

				if (choice == 'y' || choice == 'Y') {
					assignedDriverName = d.name;
					driverAccepted = true;
				} else if (choice == 'n' || choice == 'N') {
					currentDriverIndex++;
				} else if (choice == 'q' || choice == 'Q') {
					cout << "Ride booking cancelled.\n";
					break;
				} else {
					cout << "Invalid input.\n";
				}
			}
//------------------------------------------------------------------------------------------------------------------------------------------------
			if (driverAccepted) {
				char finalConfirm;
				cout << "\nConfirm booking with " << assignedDriverName << " (y/n)? ";
				cin >> finalConfirm;
				cin.ignore(numeric_limits<streamsize>::max(), '\n');

				if (finalConfirm == 'y' || finalConfirm == 'Y') {
					driverInfoMap[assignedDriverName].location = currentRequest.drop;

					currentUser.rideHistory.addRide(Ride(currentRequest.userEmail, assignedDriverName,currentRequest.pickup, currentRequest.drop, userOfferedFare, ride_distance, currentRequest.rideType));
					cout << "\nRide booked and recorded.\n";
				} else {
					cout << "\nRide cancelled.\n";
				}
			}

		} catch (const exception& e) {
			cout << "Error: " << e.what() << endl;
		}
		break;
	}

	case 2:
		if (!currentLoggedInUserEmail.empty()) {
			cout << "\n--- " << userMap[currentLoggedInUserEmail].name << "'s Ride History ---\n";
			userMap[currentLoggedInUserEmail].rideHistory.displayHistory();
			cout << "-------------------------------------------\n";
		} else {
			cout <<  "\nyou have to log in to view your ride history.\n";
		}
		break;

	case 3:
		listAllLocations();
		break;
	case 4: 
		cout << "\nLogging out " << userMap[currentLoggedInUserEmail].name << ".\n";
		currentLoggedInUserEmail = "";
		break;

	case 5:
		running = false;
		break;

	default:
		cout << "Invalid choice. try again.\n";
		break;
        }
    } 
    cout << "Exiting Ride Booking System.\n";
	return 0;
}