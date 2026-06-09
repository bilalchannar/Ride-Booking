#include "RideBookingEngine.h"

unordered_map<string, User> userMap;
unordered_map<string, string> nameMap;
unordered_map<string, vector<Edge>> graph;
unordered_map<string, Driver> driverInfoMap;
unordered_map<string, Vehicle> driverVehicleMap;
unordered_map<string, int> popularRoutes;
string currentLoggedInUserEmail = "";
double surgeMultiplier = 1.0;
ZoneBST cityZones;
