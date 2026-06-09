#ifndef RIDE_BOOKING_ENGINE_H
#define RIDE_BOOKING_ENGINE_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <stack>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits>
#include <iomanip>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#endif

using namespace std;

// --- Colors ---
const string RESET = "\033[0m";
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string BLUE = "\033[34m";
const string CYAN = "\033[36m";
const string BOLD = "\033[1m";
const string WHITE = "\033[37m";

// --- Structs ---
struct Ride {
    string userEmail;
    string driverName;
    string pickup;
    string drop;
    int fare;
    int distance;
    string rideType;

    Ride(string ue, string dn, string p, string d, int f, int dist, string rt)
        : userEmail(ue), driverName(dn), pickup(p), drop(d), fare(f), distance(dist), rideType(rt) {}
};

struct RideHistory {
    vector<Ride> rides;
    void addRide(const Ride &r) { rides.push_back(r); }
    void displayHistory() {
        if (rides.empty()) {
            cout << "No ride history found." << endl;
            return;
        }
        for (const auto &r : rides) {
            cout << "From: " << r.pickup << " -> To: " << r.drop 
                 << " | Driver: " << r.driverName << " | Fare: Rs. " << r.fare << endl;
        }
    }
};

struct User {
    string email, name, phone, password;
    double balance;
    int loyaltyPoints;
    RideHistory rideHistory;

    User() : balance(0.0), loyaltyPoints(0) {}
    User(string e, string n, string p, string pass, double b = 500.0, int lp = 0)
        : email(e), name(n), phone(p), password(pass), balance(b), loyaltyPoints(lp) {}
};

struct Driver {
    string name;
    string location;
    double rating;
    int ratingCount;
    Driver() : rating(4.0), ratingCount(1) {}
    Driver(string n, string l, double r) : name(n), location(l), rating(r), ratingCount(1) {}
};

struct Vehicle {
    string noPlate, driverName, carModel, carType;
};

struct Edge {
    string to;
    int weight, time;
    Edge(string t, int w, int tm) : to(t), weight(w), time(tm) {}
};

struct AvailableDriver {
    string name;
    int distance;
    double rating;
    AvailableDriver(string n, int d, double r) : name(n), distance(d), rating(r) {}
};

struct CompareAvailableDriver {
    bool operator()(const AvailableDriver &a, const AvailableDriver &b) {
        if (a.distance != b.distance) return a.distance > b.distance;
        return a.rating < b.rating;
    }
};

// --- Global Maps ---
extern unordered_map<string, User> userMap;
extern unordered_map<string, string> nameMap;
extern unordered_map<string, vector<Edge>> graph;
extern unordered_map<string, Driver> driverInfoMap;
extern unordered_map<string, Vehicle> driverVehicleMap;
extern unordered_map<string, int> popularRoutes;
extern string currentLoggedInUserEmail;
extern double surgeMultiplier;

// --- Helper Functions ---
inline string toLowerCase(string str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

inline string normalize(string str) {
    string n = "";
    for (char c : str) { if (!isspace(c)) n += tolower(c); }
    return n;
}

inline string hashPassword(string pass) {
    if (pass.rfind("$djb2$", 0) == 0) return pass; // already hashed
    unsigned long hash = 5381;
    for (char c : pass) {
        hash = ((hash << 5) + hash) + c;
    }
    stringstream ss;
    ss << "$djb2$" << hex << hash;
    return ss.str();
}

// --- DSA Logic ---
struct ZoneNode {
    string zoneName;
    ZoneNode *left, *right;
    ZoneNode(string name) : zoneName(name), left(nullptr), right(nullptr) {}
};

class ZoneBST {
    ZoneNode *root;
    ZoneNode *insertRec(ZoneNode *node, string zone) {
        if (!node) return new ZoneNode(zone);
        if (zone < node->zoneName) node->left = insertRec(node->left, zone);
        else if (zone > node->zoneName) node->right = insertRec(node->right, zone);
        return node;
    }
    bool searchRec(ZoneNode *node, string zone) {
        if (!node) return false;
        if (zone == node->zoneName) return true;
        return (zone < node->zoneName) ? searchRec(node->left, zone) : searchRec(node->right, zone);
    }
    void inorderRec(ZoneNode *node) {
        if (!node) return;
        inorderRec(node->left);
        if (nameMap.count(node->zoneName)) cout << nameMap[node->zoneName] << endl;
        else {
            string name = node->zoneName;
            bool next = true;
            for (char &c : name) { if (isspace(c)) next = true; else if (next) { c = toupper(c); next = false; } }
            cout << name << endl;
        }
        inorderRec(node->right);
    }
    void deleteTree(ZoneNode *node) {
        if (!node) return;
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }
public:
    ZoneBST() : root(nullptr) {}
    ~ZoneBST() { deleteTree(root); }
    void insert(string zone) { root = insertRec(root, zone); }
    bool search(string zone) { return searchRec(root, zone); }
    void display() { inorderRec(root); }
};

extern ZoneBST cityZones;

inline pair<vector<string>, int> dijkstra(string start, string end, string type) {
    if (graph.find(start) == graph.end() || graph.find(end) == graph.end()) {
        return {{}, -1};
    }
    unordered_map<string, int> dist;
    unordered_map<string, string> parent;
    for (auto const &pair : graph) dist[pair.first] = numeric_limits<int>::max();
    dist[start] = 0;

    priority_queue<pair<int, string>, vector<pair<int, string>>, greater<pair<int, string>>> pq;
    pq.push({0, start});

    while (!pq.empty()) {
        string u = pq.top().second;
        int d = pq.top().first;
        pq.pop();

        if (d > dist[u]) continue;
        if (u == end) break;

        for (const auto &edge : graph[u]) {
            int weight = (type == "distance") ? edge.weight : edge.time;
            if (dist[u] + weight < dist[edge.to]) {
                dist[edge.to] = dist[u] + weight;
                parent[edge.to] = u;
                pq.push({dist[edge.to], edge.to});
            }
        }
    }

    if (dist[end] == numeric_limits<int>::max()) return {{}, -1};
    vector<string> path;
    for (string at = end; at != ""; at = parent[at]) path.push_back(at);
    reverse(path.begin(), path.end());
    return {path, dist[end]};
}

inline void buildGraph() {
    graph["F6"] = {Edge("F7", 2, 4), Edge("Blue Area", 3, 6), Edge("QAU", 6, 12)};
    graph["F7"] = {Edge("F6", 2, 4), Edge("F8", 3, 5), Edge("G7", 3, 6), Edge("Blue Area", 2, 4)};
    graph["F8"] = {Edge("F7", 3, 5), Edge("F9", 2, 4), Edge("G8", 3, 6), Edge("Air Uni", 3, 6), Edge("Faisal Mosque", 3, 6)};
    graph["F9"] = {Edge("F8", 2, 4), Edge("F10", 3, 6), Edge("G9", 2, 4), Edge("PAF Hospital", 2, 4), Edge("Air Uni", 3, 6)};
    graph["F10"] = {Edge("F9", 3, 6), Edge("G10", 2, 4), Edge("G11", 3, 6), Edge("PAF Hospital", 1, 2)};
    graph["G6"] = {Edge("G7", 2, 4), Edge("Blue Area", 2, 4), Edge("Shakarparian", 3, 6), Edge("Bani Gala", 4, 8)};
    graph["G7"] = {Edge("G6", 2, 4), Edge("G8", 2, 4), Edge("F7", 3, 6), Edge("Blue Area", 2, 4), Edge("Zero Point", 2, 4), Edge("Shakarparian", 2, 4)};
    graph["G8"] = {Edge("G7", 2, 4), Edge("G9", 2, 4), Edge("Zero Point", 2, 5), Edge("F8", 3, 6)};
    graph["G9"] = {Edge("G8", 2, 4), Edge("G10", 2, 4), Edge("F10", 3, 5), Edge("F9", 2, 4)};
    graph["G10"] = {Edge("G9", 2, 4), Edge("G11", 2, 4), Edge("F10", 2, 4)};
    graph["G11"] = {Edge("G10", 2, 4), Edge("FAST", 3, 6), Edge("NUST", 4, 8), Edge("F10", 3, 6)};
    graph["Blue Area"] = {Edge("F7", 2, 4), Edge("G7", 2, 4), Edge("G6", 2, 4), Edge("F6", 3, 6)};
    graph["Zero Point"] = {Edge("G8", 2, 4), Edge("I8", 3, 5), Edge("G7", 2, 4)};
    graph["Shakarparian"] = {Edge("G6", 3, 6), Edge("Comsats", 5, 10), Edge("G7", 2, 4)};
    graph["Bani Gala"] = {Edge("G6", 4, 8), Edge("QAU", 6, 12), Edge("Comsats", 4, 8)};
    graph["QAU"] = {Edge("F6", 6, 12), Edge("Bani Gala", 6, 12)};
    graph["I8"] = {Edge("Zero Point", 3, 5), Edge("I9", 2, 4), Edge("Faizabad", 4, 7), Edge("Comsats", 3, 6)};
    graph["I9"] = {Edge("I8", 2, 4), Edge("I10", 2, 4)};
    graph["I10"] = {Edge("I9", 2, 4), Edge("I11", 3, 5), Edge("FAST", 2, 4), Edge("Pirwadhai", 4, 8)};
    graph["I11"] = {Edge("I10", 3, 5), Edge("I12", 3, 6)};
    graph["I12"] = {Edge("I10", 3, 5), Edge("NUST", 2, 4)};
    graph["Pirwadhai"] = {Edge("I10", 4, 8), Edge("Saddar", 5, 10)};
    graph["Faizabad"] = {Edge("I8", 4, 7), Edge("Stadium", 2, 4), Edge("Saddar", 6, 12)};
    graph["Stadium"] = {Edge("Faizabad", 2, 4), Edge("Saddar", 3, 5)};
    graph["Saddar"] = {Edge("Pirwadhai", 5, 10), Edge("Stadium", 3, 5), Edge("Faizabad", 6, 12)};
    graph["PAF Hospital"] = {Edge("F10", 1, 2), Edge("F9", 2, 4)};
    graph["Air Uni"] = {Edge("F9", 3, 6), Edge("F8", 3, 6)};
    graph["Faisal Mosque"] = {Edge("F8", 3, 6)};
    graph["FAST"] = {Edge("G11", 3, 6), Edge("I10", 2, 4)};
    graph["NUST"] = {Edge("G11", 4, 8), Edge("I12", 2, 4)};
    graph["Comsats"] = {Edge("Shakarparian", 5, 10), Edge("Bani Gala", 4, 8), Edge("I8", 3, 6)};

    for (const auto &pair : graph) {
        string norm = normalize(pair.first);
        nameMap[norm] = pair.first;
        cityZones.insert(norm);
    }
}

inline void saveData() {
    ofstream uOut("users.txt");
    for (auto const &entry : userMap) {
        const User &user = entry.second;
        uOut << user.email << "|" << user.name << "|" << user.phone << "|"
             << user.password << "|" << user.balance << "|" << user.loyaltyPoints << endl;
    }
    uOut.close();

    ofstream dOut("drivers.txt");
    for (auto const &entry : driverInfoMap) {
        const Driver &driver = entry.second;
        dOut << driver.name << "|" << driver.location << "|" << driver.rating << "|"
             << driver.ratingCount << endl;
    }
    dOut.close();

    ofstream vOut("vehicles.txt");
    for (auto const &entry : driverVehicleMap) {
        const Vehicle &veh = entry.second;
        vOut << veh.driverName << "|" << veh.noPlate << "|" << veh.carModel << "|" << veh.carType << endl;
    }
    vOut.close();
}

inline void loadData() {
    ifstream uIn("users.txt");
    string line;
    while (getline(uIn, line)) {
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);
        size_t p4 = line.find('|', p3 + 1);
        size_t p5 = line.find('|', p4 + 1);
        if (p1 != string::npos && p2 != string::npos && p3 != string::npos && p4 != string::npos) {
            string email = line.substr(0, p1);
            string name = line.substr(p1 + 1, p2 - p1 - 1);
            string phone = line.substr(p2 + 1, p3 - p2 - 1);
            string password = hashPassword(line.substr(p3 + 1, p4 - p3 - 1));
            double balance = stod(line.substr(p4 + 1, (p5 == string::npos ? string::npos : p5 - p4 - 1)));
            int lp = (p5 != string::npos) ? stoi(line.substr(p5 + 1)) : 0;
            userMap[email] = User(email, name, phone, password, balance, lp);
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
            double rating = stod(line.substr(p2 + 1, p3 - p2 - 1));
            int count = stoi(line.substr(p3 + 1));
            driverInfoMap[name] = Driver(name, loc, rating);
            driverInfoMap[name].ratingCount = count;
        }
    }
    dIn.close();

    ifstream vIn("vehicles.txt");
    while (getline(vIn, line)) {
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);
        if (p1 != string::npos && p2 != string::npos && p3 != string::npos) {
            string driverName = line.substr(0, p1);
            string noPlate = line.substr(p1 + 1, p2 - p1 - 1);
            string carModel = line.substr(p2 + 1, p3 - p2 - 1);
            string carType = line.substr(p3 + 1);
            driverVehicleMap[driverName] = Vehicle{noPlate, driverName, carModel, carType};
        }
    }
    vIn.close();
}

#endif
