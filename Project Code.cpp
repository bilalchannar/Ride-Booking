#include <algorithm>
#include <cctype>
#include <chrono>
#include <climits>
#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <queue>
#include <random>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

using namespace std;

// --- Visual Enhancements (ANSI Colors) ---
const string RESET = "\033[0m";
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string BLUE = "\033[34m";
const string CYAN = "\033[36m";
const string BOLD = "\033[1m";

string getPasswordInput(string prompt) {
	string password = "";
	char ch;
	cout << prompt;
	while (true) {
		ch = _getch();
		if (ch == 13) { // Enter key
			cout << endl;
			break;
		} else if (ch == 8) { // Backspace key
			if (password.length() > 0) {
				password.pop_back();
				cout << "\b \b";
			}
		} else if (ch == 3) { // Ctrl+C handling (optional)
			exit(0);
		} else if (isprint(ch)) { // Only add printable characters
			password += ch;
			cout << "*";
		}
	}
	return password;
}

void simulateLoading(string message) {
	cout << message;
	for (int i = 0; i < 3; ++i) {
		this_thread::sleep_for(chrono::milliseconds(500));
		cout << "." << flush;
	}
	cout << endl;
}

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
	int ratingCount;

	Driver() : rating(0.0), ratingCount(0) {}
	Driver(string n, string l, double r, int rc = 1)
		: name(n), location(l), rating(r), ratingCount(rc) {}
};

struct Ride { // Stores the full ride details
	string userEmail;
	string driverID;
	string from;
	string to;
	int fare;
	int distance;
	string rideType;

	Ride()
		: userEmail(""), driverID(""), from(""), to(""), fare(0), distance(0),
		  rideType("") {}

	Ride(string u_email, string d, string f, string t, int fa, int dis,
	     string r_type)
		: userEmail(u_email), driverID(d), from(f), to(t), fare(fa),
		  distance(dis), rideType(r_type) {}
};
struct RideNode {
	Ride ride;
	RideNode *next;

	RideNode(Ride r) : ride(r), next(nullptr) {}
};

class RideHistory {
	private:
		vector<Ride> rides;

	public:
		void addRide(const Ride &r) {
			rides.push_back(r);
		}

		void displayHistory() {
			if (rides.empty()) {
				cout << "No ride history to display.\n";
				return;
			}
			for (Ride &r : rides) {
				cout << "From " << r.from << " to " << r.to << " | Fare: Rs. " << r.fare
				     << " | Distance: " << r.distance << " km | Type: " << r.rideType
				     << endl;
			}
		}

		vector<Ride> &getRides() {
			return rides;
		}
};
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct User {
	string email;
	string name;
	string phone;
	string password;
	double balance;
	RideHistory rideHistory;

	User() : balance(0.0) {}

	User(string e, string n, string p, string pass, double b = 500.0)
		: email(e), name(n), phone(p), password(pass), balance(b), rideHistory() {
	}
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

	RideRequest(string u_email, string u_name, string p, string d,
	            string r_type) {
		userEmail = u_email, userName = u_name, pickup = p, drop = d,
		rideType = r_type;
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
	bool operator()(const AvailableDriver &d1,
	                const AvailableDriver &d2) { // not medifying the variables
		if (d1.distance == d2.distance) {
			return d1.rating > d2.rating; // min heap
		}
		return d1.distance < d2.distance;
	}
};
class DriverPriorityQueue {
	private:
		priority_queue<AvailableDriver, vector<AvailableDriver>,
		               CompareAvailableDriver>
		               pq; // vector is unerlying container to store element, or then it is
		// comapred with CAD

	public:
		void addDriver(const AvailableDriver &d) {
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

		bool empty() const {
			return pq.empty();
		}

		void displayDrivers() {
			priority_queue<AvailableDriver, vector<AvailableDriver>,
			               CompareAvailableDriver>
			               temp_pq = pq;
			if (temp_pq.empty()) {
				cout << "No available drivers in the queue." << endl;
				return;
			}
			cout << "--- Available Drivers in Priority Queue---" << endl;
			while (!temp_pq.empty()) {
				AvailableDriver d = temp_pq.top();
				temp_pq.pop();
				cout << "- Driver: " << d.name << ", Distance: " << d.distance
				     << "km, Rating: " << d.rating << endl;
			}
		}
};
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
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

unordered_map<string, User> userMap;
unordered_map<string, string> nameMap;
unordered_map<string, vector<Edge>> graph;
unordered_map<string, Driver> driverInfoMap;
unordered_map<string, Vehicle> driverVehicleMap;

string normalize(string str) {
	string n = "";
	for (char c : str) {
		if (!isspace(c)) {
			n += tolower(c);
		}
	}
	return n;
}

struct ZoneNode {
	string zoneName;
	ZoneNode *left;
	ZoneNode *right;

	ZoneNode(string name) {
		zoneName = name, left = nullptr, right = nullptr;
	}
};

class ZoneBST {
	private:
		ZoneNode *root;

		ZoneNode *insertRec(ZoneNode *node, string zone) {
			if (!node)
				return new ZoneNode(zone);
			if (zone < node->zoneName)
				node->left = insertRec(node->left, zone);
			else if (zone > node->zoneName)
				node->right = insertRec(node->right, zone);
			return node;
		}

		bool searchRec(ZoneNode *node, string zone) {
			if (!node)
				return false;
			if (zone == node->zoneName)
				return true;
			if (zone < node->zoneName)
				return searchRec(node->left, zone);
			else
				return searchRec(node->right, zone);
		}

		void inorderRec(ZoneNode *node) {
			if (!node)
				return;
			inorderRec(node->left);
			if (nameMap.count(node->zoneName)) {
				cout << nameMap[node->zoneName] << endl;
			} else {
				string name = node->zoneName;
				bool nextCap = true;
				for (char &c : name) {
					if (isspace(c)) {
						nextCap = true;
					} else if (nextCap) {
						c = toupper(c);
						nextCap = false;
					}
				}
				cout << name << endl;
			}
			inorderRec(node->right);
		}

		void deleteTree(ZoneNode *node) {
			if (!node)
				return;
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

		ZoneNode *getRoot() {
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

int nextRideID = 1;
ZoneBST cityZones;

string currentLoggedInUserEmail = "";
double surgeMultiplier = 1.0;


void checkSurge() {
	time_t now = time(0);
	tm *ltm = localtime(&now);
	if (!ltm)
		return; // Safety check
	int hour = ltm->tm_hour;
	// Peak hours: 8-10 AM and 5-8 PM
	if ((hour >= 8 && hour <= 10) || (hour >= 17 && hour <= 20)) {
		surgeMultiplier = 1.5;
		cout << YELLOW
		     << "\n[!] Peak hour detected. Surge pricing (1.5x) is active." << RESET
		     << endl;
	} else {
		surgeMultiplier = 1.0;
	}
}

void applyTraffic() {
	srand(time(0));
	for (auto &pair : graph) {
		for (auto &edge : pair.second) {
			int traffic = rand() % 3; // 0: Low, 1: Med, 2: High
			if (traffic == 1)
				edge.time += 2;
			else if (traffic == 2)
				edge.time += 5;
		}
	}
	cout << CYAN << "[System] Real-time traffic data updated." << RESET << endl;
}

void saveData() {
	ofstream uOut("users.txt");
	for (auto const &entry : userMap) {
		const User &user = entry.second;
		uOut << user.email << "|" << user.name << "|" << user.phone << "|"
		     << user.password << "|" << user.balance << endl;
	}
	uOut.close();

	ofstream dOut("drivers.txt");
	for (auto const &entry : driverInfoMap) {
		const Driver &driver = entry.second;
		dOut << driver.name << "|" << driver.location << "|" << driver.rating << "|"
		     << driver.ratingCount << endl;
	}
	dOut.close();
}

void loadData() {
	ifstream uIn("users.txt");
	string line;
	while (getline(uIn, line)) {
		size_t p1 = line.find('|');
		size_t p2 = line.find('|', p1 + 1);
		size_t p3 = line.find('|', p2 + 1);
		size_t p4 = line.find('|', p3 + 1);
		if (p1 != string::npos && p2 != string::npos && p3 != string::npos &&
		        p4 != string::npos) {
			string email = line.substr(0, p1);
			string name = line.substr(p1 + 1, p2 - p1 - 1);
			string phone = line.substr(p2 + 1, p3 - p2 - 1);
			string password = line.substr(p3 + 1, p4 - p3 - 1);
			double balance = stod(line.substr(p4 + 1));
			userMap[email] = User(email, name, phone, password, balance);
		}
	}
	uIn.close();

	ifstream dIn("drivers.txt");
	while (getline(dIn, line)) {
		size_t p1 = line.find('|');
		size_t p2 = line.find('|', p1 + 1);
		size_t p3 = line.find('|', p2 + 1);
		if (p1 != string::npos && p2 != string::npos && p3 != string::npos) {
			string name = line.substr(0, p1);
			string loc = line.substr(p1 + 1, p2 - p1 - 1);
			double rat = stod(line.substr(p2 + 1, p3 - p2 - 1));
			int count = stoi(line.substr(p3 + 1));
			driverInfoMap[name] = Driver(name, loc, rat, count);
		}
	}
	dIn.close();
}

string toLowerCase(string str) {
	string lower = str;
	transform(lower.begin(), lower.end(), lower.begin(),
	          ::tolower); // algorithm.. Converts a single character to its
	// lowercase form.
	return lower;
}

void buildGraph() {
	graph["F6"] = {Edge("F7", 2, 4), Edge("Blue Area", 3, 6), Edge("QAU", 6, 12)};
	graph["F7"] = {Edge("F6", 2, 4), Edge("F8", 2, 5), Edge("Blue Area", 2, 4)};
	graph["F8"] = {Edge("F7", 2, 5), Edge("F9", 2, 4),
	               Edge("Faisal Mosque", 3, 7), Edge("G8", 3, 6)
	              };
	graph["F9"] = {Edge("F8", 2, 4), Edge("F10", 3, 5), Edge("Air Uni", 3, 6),
	               Edge("G9", 2, 4)
	              };
	graph["F10"] = {Edge("F9", 3, 5), Edge("G9", 3, 5),
	                Edge("PAF Hospital", 1, 2), Edge("G10", 2, 4),
	                Edge("G11", 3, 6)
	               };
	graph["G6"] = {Edge("G7", 2, 4), Edge("Shakarparian", 3, 5),
	               Edge("Bani Gala", 4, 8), Edge("Blue Area", 2, 4)
	              };
	graph["G7"] = {Edge("G6", 2, 4), Edge("G8", 2, 4), Edge("Blue Area", 2, 4),
	               Edge("Zero Point", 3, 6)
	              };
	graph["G8"] = {Edge("G7", 2, 4), Edge("G9", 2, 4), Edge("Zero Point", 2, 5),
	               Edge("F8", 3, 6)
	              };
	graph["G9"] = {Edge("G8", 2, 4), Edge("G10", 2, 4), Edge("F10", 3, 5),
	               Edge("F9", 2, 4)
	              };
	graph["G10"] = {Edge("G9", 2, 4), Edge("G11", 2, 4), Edge("F10", 2, 4)};
	graph["G11"] = {Edge("G10", 2, 4), Edge("FAST", 3, 6), Edge("NUST", 4, 8),
	                Edge("F10", 3, 6)
	               };
	graph["Blue Area"] = {Edge("F7", 2, 4), Edge("G7", 2, 4), Edge("G6", 2, 4),
	                      Edge("F6", 3, 6)
	                     };
	graph["Zero Point"] = {Edge("G8", 2, 4), Edge("I8", 3, 5), Edge("G7", 2, 4)};
	graph["Shakarparian"] = {Edge("G6", 3, 6), Edge("Comsats", 5, 10),
	                         Edge("G7", 2, 4)
	                        };
	graph["Bani Gala"] = {Edge("G6", 4, 8), Edge("QAU", 6, 12),
	                      Edge("Comsats", 4, 8)
	                     };
	graph["QAU"] = {Edge("F6", 6, 12), Edge("Bani Gala", 6, 12)};
	graph["I8"] = {Edge("Zero Point", 3, 5), Edge("I9", 2, 4),
	               Edge("Faizabad", 4, 7), Edge("Comsats", 3, 6)
	              };
	graph["I9"] = {Edge("I8", 2, 4), Edge("I10", 2, 4)};
	graph["I10"] = {Edge("I9", 2, 4), Edge("I11", 3, 5), Edge("FAST", 2, 4),
	                Edge("Pirwadhai", 4, 8)
	               };
	graph["I11"] = {Edge("I10", 3, 5), Edge("I12", 3, 6)};
	graph["I12"] = {Edge("I10", 3, 5), Edge("NUST", 2, 4)};
	graph["Pirwadhai"] = {Edge("I10", 4, 8), Edge("Saddar", 5, 10)};
	graph["Faizabad"] = {Edge("I8", 4, 7), Edge("Stadium", 2, 4),
	                     Edge("Saddar", 6, 12)
	                    };
	graph["Stadium"] = {Edge("Faizabad", 2, 4), Edge("Saddar", 3, 5)};
	graph["Saddar"] = {Edge("Pirwadhai", 5, 10), Edge("Stadium", 3, 5),
	                   Edge("Faizabad", 6, 12)
	                  };
	graph["PAF Hospital"] = {Edge("F10", 1, 2), Edge("F9", 2, 4)};
	graph["Air Uni"] = {Edge("F9", 3, 6), Edge("F8", 3, 6)};
	graph["Faisal Mosque"] = {Edge("F8", 3, 6)};
	graph["FAST"] = {Edge("G11", 3, 6), Edge("I10", 2, 4)};
	graph["NUST"] = {Edge("G11", 4, 8), Edge("I12", 2, 4)};
	graph["Comsats"] = {Edge("Shakarparian", 5, 10), Edge("Bani Gala", 4, 8),
	                    Edge("I8", 3, 6)
	                   };

	for (const auto &pair : graph) {
		string norm = normalize(pair.first);
		nameMap[norm] = pair.first;
		cityZones.insert(norm);
	}
	applyTraffic();
	loadData();
}

struct ComparePair {
	bool operator()(const pair<string, int> &a, const pair<string, int> &b) {
		return a.second > b.second;
	}
};

pair<vector<string>, int> dijkstra(const string &start, const string &end,
                                   const string &mode) { // key id string
	if (graph.find(start) == graph.end() || graph.find(end) == graph.end()) {
		cerr << "Error: Start or end location not found.\n";
		return {{}, -1};
	}

	unordered_map<string, int> dist;
	unordered_map<string, string> parent;

	priority_queue<pair<string, int>, vector<pair<string, int>>, ComparePair> pq;

	for (auto &entry : graph) {
		string node = entry.first;
		dist[node] = INT_MAX;
		parent[node] = "";
	}

	dist[start] = 0;
	pq.push(make_pair(start, 0));

	while (!pq.empty()) {
		pair<string, int> top = pq.top();
		string current = top.first;
		int currDist = top.second;
		pq.pop();

		if (currDist > dist[current])
			continue;
		if (current == end)
			break;

		for (auto &edge : graph[current]) {
			int weight = (mode == "time") ? edge.time : edge.distance;
			if (dist[current] + weight < dist[edge.destination]) {
				dist[edge.destination] = dist[current] + weight;
				parent[edge.destination] = current;
				pq.push(make_pair(edge.destination, dist[edge.destination]));
			}
		}
	}

	vector<string> path;
	if (dist[end] == INT_MAX)
		return {path, -1}; // If the shortest path is still ∞, that means no way to
	// reach the destination.

	for (string at = end; at != "";
	        at = parent[at]) { // if the end point is the start point
		path.push_back(at);
		if (at == start)
			break;
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
	string user_Name, user_Email, user_Phone, user_Password;
	cout << "\n--- New User Sign Up ---\n";
	cout << "Enter your Full Name (e.g., Ali Ahmed): ";
	getline(cin, user_Name);

	bool validPhone = false;
	while (!validPhone) {
		cout << "Enter your Phone Number (11 digits, e.g., 03001234567): ";
		getline(cin, user_Phone);
		if (user_Phone.length() == 11) {
			bool allDigits = true;
			for (char c : user_Phone)
				if (!isdigit(c))
					allDigits = false;
			if (allDigits)
				validPhone = true;
			else
				cout << "Invalid phone number. Please enter only digits.\n";
		} else
			cout << "Invalid phone number. Please enter exactly 11 digits.\n";
	}

	bool validEmail = false;
	while (!validEmail) {
		cout << "Enter your Email (e.g., user@gmail.com): ";
		getline(cin, user_Email);
		string lowerEmail = toLowerCase(user_Email);
		if (lowerEmail.length() >= 10 &&
		        lowerEmail.substr(lowerEmail.length() - 10) == "@gmail.com") {
			validEmail = true;
		} else
			cout << "Invalid email address.\n";
	}

	bool validPass = false;
	while (!validPass) {
		user_Password = getPasswordInput("Create a Password (min 6 characters): ");
		string confirm_Password = getPasswordInput("Confirm your Password: ");

		if (user_Password.length() < 6) {
			cout << "Password too short! Must be at least 6 characters.\n";
		} else if (user_Password.find(' ') != string::npos) {
			cout << "Password cannot contain spaces.\n";
		} else if (user_Password != confirm_Password) {
			cout << RED << "Passwords do not match! Please try again." << RESET
			     << endl;
		} else {
			validPass = true;
		}
	}

	string lowerEmail = toLowerCase(user_Email);
	if (userMap.count(lowerEmail)) {
		cout << "Error: Account already exists.\n";
	} else {
		userMap[lowerEmail] =
		    User(lowerEmail, user_Name, user_Phone, user_Password);
		cout << GREEN << "\nAccount created successfully!" << RESET << endl;
		currentLoggedInUserEmail = lowerEmail;
		saveData();
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void handle_Login() {
	string emailInput, passInput;
	cout << "\n--- User Log In ---\n";
	cout << "Enter your email (e.g., user@gmail.com): ";
	getline(cin, emailInput);
	string lowerEmail = toLowerCase(emailInput);

	if (userMap.count(lowerEmail)) {
		passInput = getPasswordInput("Enter your password: ");
		if (userMap[lowerEmail].password == passInput) {
			currentLoggedInUserEmail = lowerEmail;
			cout << GREEN << "Successfully logged in as "
			     << userMap[currentLoggedInUserEmail].name << "!" << RESET << endl;
		} else {
			cout << RED << "Incorrect password!" << RESET << endl;
		}
	} else {
		cout << RED << "Email not found." << RESET << endl;
	}
}

void handle_Admin() {
	string pass;
	cout << "\n--- Admin Access ---\n";
	pass = getPasswordInput("Enter Admin Password: ");
	if (pass == "admin123") {
		bool adminRunning = true;
		while (adminRunning) {
			cout << "\n" << BOLD << "--- Admin Panel ---" << RESET << endl;
			cout << "1. View All Users\n";
			cout << "2. View All Drivers\n";
			cout << "3. Add Driver\n";
			cout << "4. Exit Admin Panel\n";
			int choice = readInt("Choice: ");
			switch (choice) {
				case 1:
					for (auto const &entry : userMap) {
						const User &u = entry.second;
						cout << "User: " << u.name << " | Email: " << u.email
						     << " | Balance: Rs. " << u.balance << endl;
					}
					break;
				case 2:
					for (auto const &entry : driverInfoMap) {
						const Driver &d = entry.second;
						cout << "Driver: " << d.name << " | Location: " << d.location
						     << " | Rating: " << d.rating << endl;
					}
					break;
				case 3: {
					string n, l, m, p, t;
					cout << "Name: ";
					getline(cin, n);

					bool validLoc = false;
					while (!validLoc) {
						cout << "Location (Suggestions: F6, G7, Blue Area, FAST): ";
						getline(cin, l);
						string normL = normalize(l);
						if (cityZones.search(normL)) {
							l = nameMap[normL];
							validLoc = true;
						} else {
							cout << RED << "Invalid city zone! Please enter a valid location from the map." << RESET << endl;
						}
					}

					cout << "Car Model (Suggestions: Toyota Corolla, Honda Civic, Suzuki Alto, Honda 125): ";
					getline(cin, m);
					cout << "Plate No: ";
					getline(cin, p);

					bool validType = false;
					while (!validType) {
						cout << "Type (Bike / Mini / Business Class): ";
						getline(cin, t);
						if (t == "Bike" || t == "Mini" || t == "Business Class") {
							validType = true;
						} else {
							cout << RED << "Invalid type! Please enter exactly: Bike, Mini, or Business Class." << RESET << endl;
						}
					}

					driverInfoMap[n] = Driver(n, l, 4.0);
					driverVehicleMap[n] = Vehicle {p, n, m, t};
					cout << GREEN << "Driver added successfully!" << RESET << endl;
					saveData();
					break;
				}
				case 4:
					adminRunning = false;
					break;
			}
		}
	} else {
		cout << RED << "Access Denied." << RESET << endl;
	}
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
int main() {
	buildGraph();

	driverInfoMap["Ali Driver"] = Driver {"Ali Driver", "F6", 4};
	driverVehicleMap["Ali Driver"] =
	    Vehicle {"ABC123", "Ali Driver", "Toyota Corolla", "Mini"}; // Set type

	driverInfoMap["Faiza Driver"] = Driver {"Faiza Driver", "G7", 5};
	driverVehicleMap["Faiza Driver"] =
	    Vehicle {"XYZ789", "Faiza Driver", "Honda Civic", "Mini"}; // Set type

	driverInfoMap["John Driver"] = Driver {"John Driver", "I8", 3};
	driverVehicleMap["John Driver"] =
	    Vehicle {"DEF456", "John Driver", "Suzuki Alto", "Mini"}; // Set type

	driverInfoMap["Bilal Rider"] = Driver {"Bilal Rider", "F7", 4};
	driverVehicleMap["Bilal Rider"] =
	    Vehicle {"BIKE001", "Bilal Rider", "Honda 125", "Bike"};

	driverInfoMap["Ahad Driver"] = Driver {"Ahad Driver", "G6", 5};
	driverVehicleMap["Ahad Driver"] =
	    Vehicle {"LUX777", "Ahad Driver", "Mercedes C-Class", "Business Class"};

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

		cout << "\n"
		     << BOLD << CYAN << "======== Welcome, "
		     << userMap[currentLoggedInUserEmail].name
		     << " to the Ride Booking App ========" << RESET << endl;
		cout << YELLOW << "Current Balance: Rs. "
		     << userMap[currentLoggedInUserEmail].balance << RESET << endl;
		cout << "1. Book a Ride\n";
		cout << "2. View Ride History\n";
		cout << "3. Add Balance (Wallet)\n";
		cout << "4. Show All Locations\n";
		cout << "5. Admin Panel\n";
		cout << "6. Log Out\n";
		cout << "7. Exit\n";

		int menuChoice = readInt("Enter choice: ");

		switch (menuChoice) {
			case 1: {
				try {
					User &currentUser = userMap[currentLoggedInUserEmail]; // login email
					string pickupInput, dropInput;
					string originalPickup, originalDrop;
					bool validLocations = false;

					while (!validLocations) {
						cout << "Enter Pickup Location (e.g., F6, Blue Area, FAST): ";
						getline(cin, pickupInput);
						cout << "Enter Drop-off Location (e.g., G11, I8, NUST): ";
						getline(cin, dropInput);

						string normPickup = normalize(pickupInput);
						string normDrop = normalize(dropInput);

						if (!cityZones.search(normPickup)) {
							cout << "Error: Pickup location '" << pickupInput
							     << "' is not a valid city zone. Please try again." << endl;
						} else if (!cityZones.search(normDrop)) {
							cout << "Error: Drop-off location '" << dropInput
							     << "' is not a valid city zone. Please try again." << endl;
						} else {
							originalPickup =
							    nameMap[normPickup]; // Get original casing from nameMap for
							// Dijkstra and storage
							originalDrop = nameMap[normDrop];

							if (originalPickup == originalDrop) {
								cout << "Error: Pickup and drop-off locations cannot be the "
								     "same. Please try again."
								     << endl;
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
						int typeChoice = readInt("Enter choice (1-3): ");

						switch (typeChoice) {
							case 1:
								preferredRideType = "Bike";
								validRideType = true;
								break;
							case 2:
								preferredRideType = "Mini";
								validRideType = true;
								break;
							case 3:
								preferredRideType = "Business Class";
								validRideType = true;
								break;
							default:
								cout << "Invalid choice. Please enter 1, 2, or 3.\n";
								break;
						}
					}
					cout << "\nYou selected: " << preferredRideType << ".\n";
					checkSurge();

					RideRequest currentRequest(currentUser.email, currentUser.name,
					                           originalPickup, originalDrop,
					                           preferredRideType);
					cout << "\nProcessing request for " << currentRequest.userName
					     << " from " << currentRequest.pickup << " to "
					     << currentRequest.drop << " (" << currentRequest.rideType << ")\n";

					pair<vector<string>, int> res_dist =
					    dijkstra(currentRequest.pickup, currentRequest.drop, "distance");
					vector<string> path = res_dist.first;
					int ride_distance = res_dist.second;

					pair<vector<string>, int> res_time =
					    dijkstra(currentRequest.pickup, currentRequest.drop, "time");
					int ride_time = res_time.second;

					if (ride_distance == -1) {
						cout << "\nNo route found from " << currentRequest.pickup << " to "
						     << currentRequest.drop << endl;
						break;
					}

					cout << "\nShortest Path (Distance): ";
					for (int i = 0; i < path.size(); ++i) {
						cout << path[i] << (i == path.size() - 1 ? "" : " -> ");
					}
					cout << "\n\nTotal Distance: " << ride_distance << " km\n";
					cout << "Estimated Time: " << ride_time << " minutes\n";
					// --- DYNAMIC FARE CALCULATION BASED ON RIDE TYPE
					// ----------------------------------------------------------------------------------------------------------------------
					int suggestedFare;
					if (preferredRideType == "Bike")
						suggestedFare = ride_distance * 15;
					else if (preferredRideType == "Mini")
						suggestedFare = ride_distance * 20;
					else
						suggestedFare = ride_distance * 35;

					suggestedFare *= surgeMultiplier;

					cout << "\nSuggested Approximate Fare: Rs. " << suggestedFare << endl;

					cout << "Do you have a promo code? (e.g., SAVE10 or 'no'): ";
					string promo;
					getline(cin, promo);
					if (promo == "SAVE10") {
						suggestedFare *= 0.9;
						cout << GREEN << "Promo applied! 10% discount." << RESET << endl;
					}

					int userOfferedFare;
					const double TOLERANCE = 0.05;
					bool fareAccepted = false;

					int minFare = static_cast<int>(suggestedFare * (1.0 - TOLERANCE));
					int maxFare = static_cast<int>(suggestedFare * (1.0 + TOLERANCE));

					while (!fareAccepted) {
						userOfferedFare = readInt("Enter the maximum fare you are willing to pay (Rs.): ");

						if (userOfferedFare >= minFare && userOfferedFare <= maxFare) {
							fareAccepted = true;
							cout << "Your offered fare (Rs. " << userOfferedFare
							     << ") is acceptable.\n";
						} else {
							cout << "Your offered fare is not within the acceptable range (Rs. "
							     << minFare << " - " << maxFare << ").\n";
						}
					}
					// --- Driver Assignment Logic (Iterative Selection - NOW FILTERS BY
					// RIDE TYPE)
					// ------------------------------------------------------------------------------------
					vector<AvailableDriver> potentialDrivers;
					for (const pair<string, Driver> &entry : driverInfoMap) {
						Driver driver = entry.second;
						if (driverVehicleMap.count(driver.name)) {
							Vehicle vehicle = driverVehicleMap[driver.name];
							if (vehicle.carType == preferredRideType) {
								pair<vector<string>, int> d_res =
								    dijkstra(driver.location, currentRequest.pickup, "distance");
								int d_dist = d_res.second;
								if (d_dist != -1) {
									potentialDrivers.push_back(
									    AvailableDriver(driver.name, d_dist, driver.rating));
								}
							}
						}
					}

					if (potentialDrivers.empty()) {
						cout << "\nNo suitable drivers found for ride type ("
						     << preferredRideType << "). Try again later.\n";
						break;
					}
					//---------------------------------------------------------------------------------------------------------------------------------------------------
					sort(potentialDrivers.begin(), potentialDrivers.end(),
					     CompareAvailableDriver());

					string assignedDriverName = "";
					bool driverAccepted = false;
					int currentDriverIndex = 0;

					while (!driverAccepted &&
					        currentDriverIndex < potentialDrivers.size()) {
						AvailableDriver d = potentialDrivers[currentDriverIndex];
						cout << "\nDriver: " << d.name << " | Distance: " << d.distance
						     << " km | Rating: " << d.rating << endl;

						if (driverVehicleMap.count(d.name)) {
							Vehicle v = driverVehicleMap[d.name];
							cout << "Vehicle: " << v.carType << " - " << v.carModel
							     << " (Plate: " << v.noPlate << ")\n";
							pair<vector<string>, int> w_res = dijkstra(
							                                      driverInfoMap[d.name].location, currentRequest.pickup, "time");
							int wait_time = w_res.second;
							cout << CYAN << "Estimated Wait Time: " << wait_time << " minutes"
							     << RESET << endl;
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
							if (currentUser.balance < userOfferedFare) {
								cout << RED
								     << "Insufficient balance! Please add funds to your wallet."
								     << RESET << endl;
							} else {
								simulateLoading("Driver is arriving");
								currentUser.balance -= userOfferedFare;
								driverInfoMap[assignedDriverName].location = currentRequest.drop;

								currentUser.rideHistory.addRide(Ride(
								                                    currentRequest.userEmail, assignedDriverName,
								                                    currentRequest.pickup, currentRequest.drop, userOfferedFare,
								                                    ride_distance, currentRequest.rideType));
								cout << GREEN << "\nRide completed! Rs. " << userOfferedFare
								     << " deducted from wallet." << RESET << endl;

								double userRating = 0;
								while (true) {
									userRating = readDouble("Rate your driver (1-5 stars): ");
									if (userRating >= 1.0 && userRating <= 5.0) break;
									cout << RED << "Invalid rating. Please enter a value between 1 and 5." << RESET << endl;
								}
								Driver &d = driverInfoMap[assignedDriverName];
								d.rating = ((d.rating * d.ratingCount) + userRating) /
								           (d.ratingCount + 1);
								d.ratingCount++;
								cout << "Thank you for rating!" << endl;
								saveData();
							}
						} else {
							cout << "\nRide cancelled.\n";
						}
					}

				} catch (const exception &e) {
					cout << "Error: " << e.what() << endl;
				}
				break;
			}

			case 2:
				if (!currentLoggedInUserEmail.empty()) {
					cout << "\n--- " << userMap[currentLoggedInUserEmail].name
					     << "'s Ride History ---\n";
					userMap[currentLoggedInUserEmail].rideHistory.displayHistory();
					cout << "-------------------------------------------\n";
				} else {
					cout << "\nyou have to log in to view your ride history.\n";
				}
				break;

			case 3: {
				double amount = 0;
				while (true) {
					amount = readDouble("Enter amount to add to wallet (e.g., 500): Rs. ");
					if (amount > 0) break;
					cout << RED << "Amount must be positive!" << RESET << endl;
				}
				userMap[currentLoggedInUserEmail].balance += amount;
				cout << GREEN << "Balance updated! New balance: Rs. "
				     << userMap[currentLoggedInUserEmail].balance << RESET << endl;
				saveData();
				break;
			}

			case 4:
				listAllLocations();
				break;

			case 5:
				handle_Admin();
				break;

			case 6:
				cout << "\nLogging out " << userMap[currentLoggedInUserEmail].name
				     << ".\n";
				currentLoggedInUserEmail = "";
				break;

			case 7:
				saveData();
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
