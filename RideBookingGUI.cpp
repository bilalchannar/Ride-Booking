#include "RideBookingEngine.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

// --- Global GUI State ---
HWND hMainWnd;
enum AppState { STATE_LOGIN, STATE_REGISTER, STATE_DASHBOARD, STATE_HISTORY, STATE_WALLET, STATE_MAP, STATE_ADMIN };
AppState currentState = STATE_LOGIN;

// Control HWNDs
HWND hLoginEmail, hLoginPass, hLoginBtn, hGoRegisterBtn;
HWND hRegName, hRegEmail, hRegPhone, hRegPass, hRegBtn, hGoLoginBtn;
HWND hPickupEdit, hDropEdit, hRechargeEdit, hUserOfferedFareEdit;
HWND hCalculateBtn, hBookBtn, hRechargeBtn, hNextDriverBtn;
HWND hRideTypeCombo;

// Admin Panel HWNDs
HWND hAdminPassEdit, hAdminUnlockBtn;
HWND hAdminSubBtn0, hAdminSubBtn1, hAdminSubBtn2, hAdminSubBtn3;
HWND hAdminDName, hAdminDLoc, hAdminDModel, hAdminDPlate, hAdminDTypeCombo, hAdminAddDriverBtn;

// Booking State Variables
bool routeCalculated = false;
vector<string> currentRoutePath;
int currentRouteDistance = -1;
int currentRouteFare = 0;
string currentRideType = "";
string pickupLoc = "", dropLoc = "";
vector<AvailableDriver> currentDrivers;
int activeDriverIdx = -1;
string bookingStatus = "";

// Admin State Variables
bool adminUnlocked = false;
int adminSubState = 0; // 0 = Users, 1 = Drivers, 2 = Add Driver, 3 = Analytics

// Animation Variables
int sidebarX = -220;    
int mainAlpha = 0;      

// --- GDI+ Helper Functions ---
void DrawString(Graphics& g, string text, int x, int y, int size, Color color, bool bold = false) {
    wstring wtext(text.begin(), text.end());
    FontFamily fontFamily(L"Segoe UI");
    Font font(&fontFamily, (REAL)size, bold ? FontStyleBold : FontStyleRegular, UnitPixel);
    SolidBrush brush(color);
    g.DrawString(wtext.c_str(), -1, &font, PointF((REAL)x, (REAL)y), &brush);
}

// --- City Zones Helper ---
vector<string> getAllZones() {
    vector<string> vec;
    for (auto const &pair : graph) {
        vec.push_back(pair.first);
    }
    sort(vec.begin(), vec.end());
    return vec;
}

// --- Control Visibility Helper ---
void UpdateControlVisibility() {
    int showLogin = (currentState == STATE_LOGIN) ? SW_SHOW : SW_HIDE;
    int showRegister = (currentState == STATE_REGISTER) ? SW_SHOW : SW_HIDE;
    int showDashboard = (currentState == STATE_DASHBOARD) ? SW_SHOW : SW_HIDE;
    int showWallet = (currentState == STATE_WALLET) ? SW_SHOW : SW_HIDE;
    int showMap = (currentState == STATE_MAP) ? SW_SHOW : SW_HIDE;
    int showAdmin = (currentState == STATE_ADMIN) ? SW_SHOW : SW_HIDE;

    int showAdminUnlock = (showAdmin == SW_SHOW && !adminUnlocked) ? SW_SHOW : SW_HIDE;
    int showAdminDash = (showAdmin == SW_SHOW && adminUnlocked) ? SW_SHOW : SW_HIDE;
    int showAdminAddForm = (showAdminDash == SW_SHOW && adminSubState == 2) ? SW_SHOW : SW_HIDE;

    // Login Form controls
    ShowWindow(hLoginEmail, showLogin);
    ShowWindow(hLoginPass, showLogin);
    ShowWindow(hLoginBtn, showLogin);
    ShowWindow(hGoRegisterBtn, showLogin);

    // Register Form controls
    ShowWindow(hRegName, showRegister);
    ShowWindow(hRegEmail, showRegister);
    ShowWindow(hRegPhone, showRegister);
    ShowWindow(hRegPass, showRegister);
    ShowWindow(hRegBtn, showRegister);
    ShowWindow(hGoLoginBtn, showRegister);

    // Dashboard controls
    ShowWindow(hPickupEdit, showDashboard);
    ShowWindow(hDropEdit, showDashboard);
    ShowWindow(hRideTypeCombo, showDashboard);
    ShowWindow(hCalculateBtn, showDashboard);
    ShowWindow(hBookBtn, showDashboard);
    ShowWindow(hUserOfferedFareEdit, showDashboard);
    ShowWindow(hNextDriverBtn, (showDashboard == SW_SHOW && routeCalculated && currentDrivers.size() > 1) ? SW_SHOW : SW_HIDE);

    // Wallet controls
    ShowWindow(hRechargeEdit, showWallet);
    ShowWindow(hRechargeBtn, showWallet);

    // Admin Unlock Screen controls
    ShowWindow(hAdminPassEdit, showAdminUnlock);
    ShowWindow(hAdminUnlockBtn, showAdminUnlock);

    // Admin Dashboard Sub-tab controls
    ShowWindow(hAdminSubBtn0, showAdminDash);
    ShowWindow(hAdminSubBtn1, showAdminDash);
    ShowWindow(hAdminSubBtn2, showAdminDash);
    ShowWindow(hAdminSubBtn3, showAdminDash);

    // Admin Add Driver Form controls
    ShowWindow(hAdminDName, showAdminAddForm);
    ShowWindow(hAdminDLoc, showAdminAddForm);
    ShowWindow(hAdminDModel, showAdminAddForm);
    ShowWindow(hAdminDPlate, showAdminAddForm);
    ShowWindow(hAdminDTypeCombo, showAdminAddForm);
    ShowWindow(hAdminAddDriverBtn, showAdminAddForm);
}

// --- Window Procedure ---
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            SetTimer(hwnd, 1, 10, NULL); // Animation timer

            // 1. Create Login Controls
            hLoginEmail = CreateWindow("EDIT", "", WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 270, 240, 260, 25, hwnd, (HMENU)111, GetModuleHandle(NULL), NULL);
            hLoginPass = CreateWindow("EDIT", "", WS_CHILD | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL, 270, 310, 260, 25, hwnd, (HMENU)112, GetModuleHandle(NULL), NULL);
            hLoginBtn = CreateWindow("BUTTON", "LOGIN", WS_CHILD, 270, 370, 260, 35, hwnd, (HMENU)113, GetModuleHandle(NULL), NULL);
            hGoRegisterBtn = CreateWindow("BUTTON", "Need an account? Sign Up", WS_CHILD | BS_FLAT, 270, 420, 260, 30, hwnd, (HMENU)114, GetModuleHandle(NULL), NULL);

            // 2. Create Register Controls
            hRegName = CreateWindow("EDIT", "", WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 270, 210, 260, 25, hwnd, (HMENU)115, GetModuleHandle(NULL), NULL);
            hRegEmail = CreateWindow("EDIT", "", WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 270, 270, 260, 25, hwnd, (HMENU)116, GetModuleHandle(NULL), NULL);
            hRegPhone = CreateWindow("EDIT", "", WS_CHILD | WS_BORDER | ES_NUMBER | ES_AUTOHSCROLL, 270, 330, 260, 25, hwnd, (HMENU)117, GetModuleHandle(NULL), NULL);
            hRegPass = CreateWindow("EDIT", "", WS_CHILD | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL, 270, 390, 260, 25, hwnd, (HMENU)118, GetModuleHandle(NULL), NULL);
            hRegBtn = CreateWindow("BUTTON", "CREATE ACCOUNT", WS_CHILD, 270, 440, 260, 35, hwnd, (HMENU)119, GetModuleHandle(NULL), NULL);
            hGoLoginBtn = CreateWindow("BUTTON", "Already have an account? Login", WS_CHILD | BS_FLAT, 270, 490, 260, 30, hwnd, (HMENU)120, GetModuleHandle(NULL), NULL);

            // 3. Create Dashboard Controls
            hPickupEdit = CreateWindow("EDIT", "", WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 400, 200, 150, 25, hwnd, (HMENU)101, GetModuleHandle(NULL), NULL);
            hDropEdit = CreateWindow("EDIT", "", WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 400, 240, 150, 25, hwnd, (HMENU)102, GetModuleHandle(NULL), NULL);
            hRideTypeCombo = CreateWindow("COMBOBOX", "", WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL, 400, 280, 150, 100, hwnd, (HMENU)103, GetModuleHandle(NULL), NULL);
            hCalculateBtn = CreateWindow("BUTTON", "Calculate Route", WS_CHILD, 570, 200, 150, 30, hwnd, (HMENU)104, GetModuleHandle(NULL), NULL);
            hBookBtn = CreateWindow("BUTTON", "Book Ride", WS_CHILD, 570, 240, 150, 30, hwnd, (HMENU)105, GetModuleHandle(NULL), NULL);
            hUserOfferedFareEdit = CreateWindow("EDIT", "", WS_CHILD | WS_BORDER | ES_NUMBER, 570, 280, 150, 25, hwnd, (HMENU)133, GetModuleHandle(NULL), NULL);
            hNextDriverBtn = CreateWindow("BUTTON", "Next Driver", WS_CHILD, 570, 320, 150, 30, hwnd, (HMENU)132, GetModuleHandle(NULL), NULL);

            // 4. Create Wallet Controls
            hRechargeEdit = CreateWindow("EDIT", "", WS_CHILD | WS_BORDER | ES_NUMBER, 400, 220, 150, 25, hwnd, (HMENU)106, GetModuleHandle(NULL), NULL);
            hRechargeBtn = CreateWindow("BUTTON", "Recharge Wallet", WS_CHILD, 260, 260, 150, 30, hwnd, (HMENU)107, GetModuleHandle(NULL), NULL);

            // 5. Create Admin Controls
            hAdminPassEdit = CreateWindow("EDIT", "", WS_CHILD | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL, 450, 220, 150, 25, hwnd, (HMENU)134, GetModuleHandle(NULL), NULL);
            hAdminUnlockBtn = CreateWindow("BUTTON", "UNLOCK ADMIN PANEL", WS_CHILD, 270, 260, 330, 30, hwnd, (HMENU)121, GetModuleHandle(NULL), NULL);

            hAdminSubBtn0 = CreateWindow("BUTTON", "View Users", WS_CHILD, 260, 100, 110, 30, hwnd, (HMENU)122, GetModuleHandle(NULL), NULL);
            hAdminSubBtn1 = CreateWindow("BUTTON", "View Drivers", WS_CHILD, 380, 100, 110, 30, hwnd, (HMENU)123, GetModuleHandle(NULL), NULL);
            hAdminSubBtn2 = CreateWindow("BUTTON", "Add Driver", WS_CHILD, 500, 100, 110, 30, hwnd, (HMENU)124, GetModuleHandle(NULL), NULL);
            hAdminSubBtn3 = CreateWindow("BUTTON", "Analytics", WS_CHILD, 620, 100, 110, 30, hwnd, (HMENU)125, GetModuleHandle(NULL), NULL);

            hAdminDName = CreateWindow("EDIT", "", WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 420, 160, 200, 25, hwnd, (HMENU)126, GetModuleHandle(NULL), NULL);
            hAdminDLoc = CreateWindow("EDIT", "", WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 420, 200, 200, 25, hwnd, (HMENU)127, GetModuleHandle(NULL), NULL);
            hAdminDModel = CreateWindow("EDIT", "", WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 420, 240, 200, 25, hwnd, (HMENU)128, GetModuleHandle(NULL), NULL);
            hAdminDPlate = CreateWindow("EDIT", "", WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 420, 280, 200, 25, hwnd, (HMENU)129, GetModuleHandle(NULL), NULL);
            hAdminDTypeCombo = CreateWindow("COMBOBOX", "", WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL, 420, 320, 200, 100, hwnd, (HMENU)130, GetModuleHandle(NULL), NULL);
            hAdminAddDriverBtn = CreateWindow("BUTTON", "ADD NEW DRIVER RECORD", WS_CHILD, 270, 370, 350, 35, hwnd, (HMENU)131, GetModuleHandle(NULL), NULL);

            SendMessage(hAdminDTypeCombo, CB_ADDSTRING, 0, (LPARAM)"Bike");
            SendMessage(hAdminDTypeCombo, CB_ADDSTRING, 0, (LPARAM)"Mini");
            SendMessage(hAdminDTypeCombo, CB_ADDSTRING, 0, (LPARAM)"Business Class");
            SendMessage(hAdminDTypeCombo, CB_SETCURSEL, 1, 0);

            // Populate ride combo box
            SendMessage(hRideTypeCombo, CB_ADDSTRING, 0, (LPARAM)"Bike");
            SendMessage(hRideTypeCombo, CB_ADDSTRING, 0, (LPARAM)"Mini");
            SendMessage(hRideTypeCombo, CB_ADDSTRING, 0, (LPARAM)"Business Class");
            SendMessage(hRideTypeCombo, CB_SETCURSEL, 1, 0); // default to Mini

            // Set sleek Segoe UI font
            HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
            SendMessage(hLoginEmail, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hLoginPass, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hLoginBtn, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hGoRegisterBtn, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hRegName, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hRegEmail, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hRegPhone, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hRegPass, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hRegBtn, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hGoLoginBtn, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hPickupEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hDropEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hRideTypeCombo, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hCalculateBtn, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hBookBtn, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hUserOfferedFareEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hNextDriverBtn, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hRechargeEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hRechargeBtn, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hAdminPassEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hAdminUnlockBtn, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hAdminSubBtn0, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hAdminSubBtn1, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hAdminSubBtn2, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hAdminSubBtn3, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hAdminDName, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hAdminDLoc, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hAdminDModel, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hAdminDPlate, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hAdminDTypeCombo, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hAdminAddDriverBtn, WM_SETFONT, (WPARAM)hFont, TRUE);

            UpdateControlVisibility();
            return 0;
        }
        case WM_CTLCOLOREDIT: {
            HDC hdcStatic = (HDC)wParam;
            SetTextColor(hdcStatic, RGB(255, 255, 255));
            SetBkColor(hdcStatic, RGB(30, 30, 45));
            static HBRUSH hBrushEdit = CreateSolidBrush(RGB(30, 30, 45));
            return (INT_PTR)hBrushEdit;
        }
        case WM_CTLCOLORLISTBOX: {
            HDC hdcList = (HDC)wParam;
            SetTextColor(hdcList, RGB(255, 255, 255));
            SetBkColor(hdcList, RGB(30, 30, 45));
            static HBRUSH hBrushList = CreateSolidBrush(RGB(30, 30, 45));
            return (INT_PTR)hBrushList;
        }
        case WM_TIMER: {
            bool needsRedraw = false;
            if (currentState != STATE_LOGIN && currentState != STATE_REGISTER) {
                if (sidebarX < 0) { sidebarX += 15; needsRedraw = true; } 
                if (mainAlpha < 255) { mainAlpha += 20; if (mainAlpha > 255) mainAlpha = 255; needsRedraw = true; } 
            }
            if (needsRedraw) InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        case WM_LBUTTONDOWN: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            
            // SIDEBAR CLICK LOGIC (Common to all dashboard screens)
            if (currentState != STATE_LOGIN && currentState != STATE_REGISTER && x >= 0 && x <= 220) {
                if (y >= 110 && y <= 150) { 
                    currentState = STATE_DASHBOARD; mainAlpha = 0;
                }
                else if (y >= 170 && y <= 210) { 
                    currentState = STATE_HISTORY; mainAlpha = 0;
                }
                else if (y >= 230 && y <= 270) { 
                    currentState = STATE_WALLET; mainAlpha = 0;
                }
                else if (y >= 290 && y <= 330) { // Map Tab
                    currentState = STATE_MAP; mainAlpha = 0;
                }
                else if (y >= 350 && y <= 390) { // Admin Tab
                    currentState = STATE_ADMIN; mainAlpha = 0;
                }
                else if (y >= 410 && y <= 450) { // Log Out option
                    currentLoggedInUserEmail = "";
                    currentState = STATE_LOGIN;
                    adminUnlocked = false; // relock admin on logout
                    SetWindowText(hLoginEmail, "");
                    SetWindowText(hLoginPass, "");
                    SetWindowText(hRegName, "");
                    SetWindowText(hRegEmail, "");
                    SetWindowText(hRegPhone, "");
                    SetWindowText(hRegPass, "");
                }
                UpdateControlVisibility();
                InvalidateRect(hwnd, NULL, TRUE);
            }
            return 0;
        }
        case WM_COMMAND: {
            int controlID = LOWORD(wParam);
            
            if (controlID == 113) { // Login Submit Button
                char emailBuf[64], passBuf[64];
                GetWindowText(hLoginEmail, emailBuf, 64);
                GetWindowText(hLoginPass, passBuf, 64);
                
                string email = emailBuf;
                string pass = passBuf;
                string lowerEmail = toLowerCase(email);
                
                if (email.empty() || pass.empty()) {
                    MessageBox(hwnd, "Please enter both Email and Password.", "Login Validation", MB_OK | MB_ICONERROR);
                } else if (userMap.count(lowerEmail) == 0) {
                    MessageBox(hwnd, "This email is not registered.", "Login Validation", MB_OK | MB_ICONERROR);
                } else {
                    if (userMap[lowerEmail].password == hashPassword(pass)) {
                        currentLoggedInUserEmail = lowerEmail;
                        currentState = STATE_DASHBOARD;
                        sidebarX = -220; mainAlpha = 0; // Trigger slide transition
                        
                        SetWindowText(hLoginEmail, "");
                        SetWindowText(hLoginPass, "");
                        
                        UpdateControlVisibility();
                        InvalidateRect(hwnd, NULL, TRUE);
                    } else {
                        MessageBox(hwnd, "Incorrect password. Please try again.", "Login Validation", MB_OK | MB_ICONERROR);
                    }
                }
            }
            else if (controlID == 114) { // Switch to Register Form
                currentState = STATE_REGISTER;
                UpdateControlVisibility();
                InvalidateRect(hwnd, NULL, TRUE);
            }
            else if (controlID == 120) { // Switch to Login Form
                currentState = STATE_LOGIN;
                UpdateControlVisibility();
                InvalidateRect(hwnd, NULL, TRUE);
            }
            else if (controlID == 119) { // Register Submit Button
                char nameBuf[64], emailBuf[64], phoneBuf[32], passBuf[64];
                GetWindowText(hRegName, nameBuf, 64);
                GetWindowText(hRegEmail, emailBuf, 64);
                GetWindowText(hRegPhone, phoneBuf, 32);
                GetWindowText(hRegPass, passBuf, 64);

                string name = nameBuf;
                string email = emailBuf;
                string phone = phoneBuf;
                string pass = passBuf;
                string lowerEmail = toLowerCase(email);

                if (name.empty() || email.empty() || phone.empty() || pass.empty()) {
                    MessageBox(hwnd, "Please fill in all registration fields.", "Registration Validation", MB_OK | MB_ICONERROR);
                }
                else if (lowerEmail.length() < 10 || lowerEmail.substr(lowerEmail.length() - 10) != "@gmail.com") {
                    MessageBox(hwnd, "Invalid email. Must end with '@gmail.com'.", "Registration Validation", MB_OK | MB_ICONERROR);
                }
                else if (phone.length() != 11) {
                    MessageBox(hwnd, "Invalid phone number. Must be exactly 11 digits.", "Registration Validation", MB_OK | MB_ICONERROR);
                }
                else {
                    bool allDigits = true;
                    for (char c : phone) {
                        if (!isdigit(c)) { allDigits = false; break; }
                    }
                    if (!allDigits) {
                        MessageBox(hwnd, "Phone number must contain only numerical digits.", "Registration Validation", MB_OK | MB_ICONERROR);
                    }
                    else if (userMap.count(lowerEmail)) {
                        MessageBox(hwnd, "An account with this email already exists.", "Registration Validation", MB_OK | MB_ICONERROR);
                    }
                    else {
                        // Create user, hash password and save database
                        userMap[lowerEmail] = User(lowerEmail, name, phone, hashPassword(pass));
                        saveData();

                        // Clear inputs
                        SetWindowText(hRegName, "");
                        SetWindowText(hRegEmail, "");
                        SetWindowText(hRegPhone, "");
                        SetWindowText(hRegPass, "");

                        currentLoggedInUserEmail = lowerEmail;
                        currentState = STATE_DASHBOARD;
                        sidebarX = -220; mainAlpha = 0; // Trigger slide transitions
                        
                        UpdateControlVisibility();
                        InvalidateRect(hwnd, NULL, TRUE);
                        MessageBox(hwnd, "Account successfully created! Welcome.", "Registration Success", MB_OK | MB_ICONINFORMATION);
                    }
                }
            }
            else if (controlID == 104) { // Calculate Route
                char pickupBuf[64], dropBuf[64];
                GetWindowText(hPickupEdit, pickupBuf, 64);
                GetWindowText(hDropEdit, dropBuf, 64);
                
                pickupLoc = pickupBuf;
                dropLoc = dropBuf;
                string normP = normalize(pickupLoc);
                string normD = normalize(dropLoc);
                
                if (cityZones.search(normP) && cityZones.search(normD)) {
                    if (nameMap[normP] == nameMap[normD]) {
                        bookingStatus = "Error: Pickup and Drop-off locations cannot be the same.";
                        routeCalculated = false;
                    } else {
                        pickupLoc = nameMap[normP];
                        dropLoc = nameMap[normD];
                        
                        int typeSel = SendMessage(hRideTypeCombo, CB_GETCURSEL, 0, 0);
                        if (typeSel == 0) currentRideType = "Bike";
                        else if (typeSel == 1) currentRideType = "Mini";
                        else currentRideType = "Business Class";
                        
                        auto res = dijkstra(pickupLoc, dropLoc, "distance");
                        if (res.second != -1) {
                            currentRouteDistance = res.second;
                            currentRoutePath = res.first;
                            
                            int baseFare = 0;
                            if (currentRideType == "Bike") baseFare = currentRouteDistance * 15;
                            else if (currentRideType == "Mini") baseFare = currentRouteDistance * 20;
                            else baseFare = currentRouteDistance * 35;
                            
                            currentRouteFare = static_cast<int>(baseFare * surgeMultiplier);
                            routeCalculated = true;
                            
                            SetWindowText(hUserOfferedFareEdit, to_string(currentRouteFare).c_str());

                            // Find matching drivers
                            currentDrivers.clear();
                            for (const auto &pair : driverInfoMap) {
                                Driver driver = pair.second;
                                if (driverVehicleMap.count(driver.name)) {
                                    Vehicle vehicle = driverVehicleMap[driver.name];
                                    if (vehicle.carType == currentRideType) {
                                        auto d_res = dijkstra(driver.location, pickupLoc, "distance");
                                        if (d_res.second != -1) {
                                            currentDrivers.push_back(AvailableDriver(driver.name, d_res.second, driver.rating));
                                        }
                                    }
                                }
                            }
                            
                            if (currentDrivers.empty()) {
                                activeDriverIdx = -1;
                                bookingStatus = "No drivers found for " + currentRideType + ".";
                            } else {
                                sort(currentDrivers.begin(), currentDrivers.end(), [](const AvailableDriver &a, const AvailableDriver &b) {
                                    if (a.distance != b.distance) return a.distance < b.distance;
                                    return a.rating > b.rating;
                                });
                                activeDriverIdx = 0;
                                bookingStatus = "Driver: " + currentDrivers[0].name + " (" + to_string(currentDrivers[0].distance) + " km away). Click Book Ride.";
                            }
                        } else {
                            bookingStatus = "Error: No route found between locations.";
                            routeCalculated = false;
                        }
                    }
                } else {
                    bookingStatus = "Error: Invalid locations entered.";
                    routeCalculated = false;
                }
                UpdateControlVisibility();
                InvalidateRect(hwnd, NULL, TRUE);
            }
            else if (controlID == 132) { // Next Driver Button
                if (routeCalculated && !currentDrivers.empty()) {
                    activeDriverIdx = (activeDriverIdx + 1) % currentDrivers.size();
                    bookingStatus = "Driver: " + currentDrivers[activeDriverIdx].name + " (" + to_string(currentDrivers[activeDriverIdx].distance) + " km away). Click Book Ride.";
                    InvalidateRect(hwnd, NULL, TRUE);
                }
            }
            else if (controlID == 105) { // Book Ride
                if (!routeCalculated || activeDriverIdx == -1 || currentDrivers.empty()) {
                    MessageBox(hwnd, "Please calculate a valid route first.", "Ride Booking", MB_OK | MB_ICONWARNING);
                } else {
                    char fareBuf[32];
                    GetWindowText(hUserOfferedFareEdit, fareBuf, 32);
                    int offered = atoi(fareBuf);

                    const double TOLERANCE = 0.05;
                    if (offered < currentRouteFare * (1.0 - TOLERANCE) || offered > currentRouteFare * (1.0 + TOLERANCE)) {
                        MessageBox(hwnd, "Your offered fare is not within the acceptable 5% range of the recommended fare.", "Fare Negotiation", MB_OK | MB_ICONWARNING);
                    } else {
                        User& u = userMap[currentLoggedInUserEmail];
                        if (u.balance >= offered) {
                            u.balance -= offered;
                            u.loyaltyPoints += offered / 10;
                            
                            string dName = currentDrivers[activeDriverIdx].name;
                            driverInfoMap[dName].location = dropLoc;
                            
                            popularRoutes[pickupLoc + " to " + dropLoc]++;
                            u.rideHistory.addRide(Ride(u.email, dName, pickupLoc, dropLoc, offered, currentRouteDistance, currentRideType));
                            
                            bookingStatus = "Success: Ride completed with " + dName + "!";
                            saveData();
                            routeCalculated = false;
                            
                            SetWindowText(hPickupEdit, "");
                            SetWindowText(hDropEdit, "");
                            SetWindowText(hUserOfferedFareEdit, "");
                        } else {
                            bookingStatus = "Error: Insufficient balance. Recharge wallet!";
                        }
                    }
                }
                UpdateControlVisibility();
                InvalidateRect(hwnd, NULL, TRUE);
            }
            else if (controlID == 107) { // Recharge
                char amountBuf[32];
                GetWindowText(hRechargeEdit, amountBuf, 32);
                double amt = atof(amountBuf);
                if (amt > 0) {
                    userMap[currentLoggedInUserEmail].balance += amt;
                    saveData();
                    SetWindowText(hRechargeEdit, "");
                    MessageBox(hwnd, "Recharge successful!", "Wallet", MB_OK | MB_ICONINFORMATION);
                } else {
                    MessageBox(hwnd, "Please enter a valid amount.", "Wallet", MB_OK | MB_ICONERROR);
                }
                InvalidateRect(hwnd, NULL, TRUE);
            }
            else if (controlID == 121) { // Unlock Admin
                char passBuf[64];
                GetWindowText(hAdminPassEdit, passBuf, 64);
                if (strcmp(passBuf, "admin123") == 0) {
                    adminUnlocked = true;
                    SetWindowText(hAdminPassEdit, "");
                    UpdateControlVisibility();
                    InvalidateRect(hwnd, NULL, TRUE);
                } else {
                    MessageBox(hwnd, "Incorrect admin password.", "Admin Access", MB_OK | MB_ICONERROR);
                }
            }
            else if (controlID == 122) { // Admin Sub Tab 0
                adminSubState = 0; UpdateControlVisibility(); InvalidateRect(hwnd, NULL, TRUE);
            }
            else if (controlID == 123) { // Admin Sub Tab 1
                adminSubState = 1; UpdateControlVisibility(); InvalidateRect(hwnd, NULL, TRUE);
            }
            else if (controlID == 124) { // Admin Sub Tab 2
                adminSubState = 2; UpdateControlVisibility(); InvalidateRect(hwnd, NULL, TRUE);
            }
            else if (controlID == 125) { // Admin Sub Tab 3
                adminSubState = 3; UpdateControlVisibility(); InvalidateRect(hwnd, NULL, TRUE);
            }
            else if (controlID == 131) { // Admin Add Driver Button
                char nameBuf[64], locBuf[64], modelBuf[64], plateBuf[64];
                GetWindowText(hAdminDName, nameBuf, 64);
                GetWindowText(hAdminDLoc, locBuf, 64);
                GetWindowText(hAdminDModel, modelBuf, 64);
                GetWindowText(hAdminDPlate, plateBuf, 64);
                int typeSel = SendMessage(hAdminDTypeCombo, CB_GETCURSEL, 0, 0);

                string name = nameBuf;
                string loc = locBuf;
                string model = modelBuf;
                string plate = plateBuf;
                string type = (typeSel == 0) ? "Bike" : (typeSel == 1 ? "Mini" : "Business Class");
                string normLoc = normalize(loc);

                if (name.empty() || loc.empty() || model.empty() || plate.empty()) {
                    MessageBox(hwnd, "Please fill in all driver fields.", "Admin Panel", MB_OK | MB_ICONERROR);
                }
                else if (!cityZones.search(normLoc)) {
                    MessageBox(hwnd, "Error: Location is not a valid city zone.", "Admin Panel", MB_OK | MB_ICONERROR);
                }
                else {
                    driverInfoMap[name] = Driver(name, nameMap[normLoc], 4.0);
                    driverVehicleMap[name] = Vehicle{plate, name, model, type};
                    saveData();

                    SetWindowText(hAdminDName, "");
                    SetWindowText(hAdminDLoc, "");
                    SetWindowText(hAdminDModel, "");
                    SetWindowText(hAdminDPlate, "");

                    MessageBox(hwnd, "Driver added successfully!", "Admin Panel", MB_OK | MB_ICONINFORMATION);
                    adminSubState = 1; // Redirect to View Drivers
                    UpdateControlVisibility();
                    InvalidateRect(hwnd, NULL, TRUE);
                }
            }
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBitmap = CreateCompatibleBitmap(hdc, 800, 600);
            SelectObject(memDC, memBitmap);
            Graphics g(memDC);
            g.SetSmoothingMode(SmoothingModeAntiAlias);

            // Background
            SolidBrush bgBrush(Color(255, 10, 10, 20));
            g.FillRectangle(&bgBrush, 0, 0, 800, 600);
            
            if (currentState == STATE_LOGIN) {
                DrawString(g, "RIDE BOOKING SYSTEM", 200, 80, 36, Color(255, 0, 255, 255), true);
                SolidBrush cardBrush(Color(200, 30, 30, 50));
                g.FillRectangle(&cardBrush, 250, 140, 300, 350);
                
                DrawString(g, "Welcome", 345, 160, 22, Color(255, 255, 255, 255), true);
                DrawString(g, "Email:", 270, 220, 12, Color(255, 220, 220, 220));
                DrawString(g, "Password:", 270, 290, 12, Color(255, 220, 220, 220));
            }
            else if (currentState == STATE_REGISTER) {
                DrawString(g, "RIDE BOOKING SYSTEM", 200, 80, 36, Color(255, 0, 255, 255), true);
                SolidBrush cardBrush(Color(200, 30, 30, 50));
                g.FillRectangle(&cardBrush, 250, 120, 300, 440);
                
                DrawString(g, "Create Account", 310, 140, 22, Color(255, 255, 255, 255), true);
                DrawString(g, "Name:", 270, 190, 11, Color(255, 220, 220, 220));
                DrawString(g, "Email:", 270, 250, 11, Color(255, 220, 220, 220));
                DrawString(g, "Phone:", 270, 310, 11, Color(255, 220, 220, 220));
                DrawString(g, "Password:", 270, 370, 11, Color(255, 220, 220, 220));
            }
            else {
                // Sidebar Background
                SolidBrush sideBrush(Color(255, 20, 20, 35));
                g.FillRectangle(&sideBrush, sidebarX, 80, 220, 520);
                
                // Sidebar Options
                DrawString(g, "DASHBOARD", 40, 30, 28, Color(mainAlpha, 0, 255, 255), true);
                DrawString(g, "Book a Ride", sidebarX + 30, 120, 18, (currentState == STATE_DASHBOARD ? Color(255, 0, 255, 255) : Color(255, 150, 150, 150)));
                DrawString(g, "History", sidebarX + 30, 180, 18, (currentState == STATE_HISTORY ? Color(255, 0, 255, 255) : Color(255, 150, 150, 150)));
                DrawString(g, "Wallet", sidebarX + 30, 240, 18, (currentState == STATE_WALLET ? Color(255, 0, 255, 255) : Color(255, 150, 150, 150)));
                DrawString(g, "City Map", sidebarX + 30, 300, 18, (currentState == STATE_MAP ? Color(255, 0, 255, 255) : Color(255, 150, 150, 150)));
                DrawString(g, "Admin Panel", sidebarX + 30, 360, 18, (currentState == STATE_ADMIN ? Color(255, 0, 255, 255) : Color(255, 150, 150, 150)));
                DrawString(g, "Log Out", sidebarX + 30, 420, 18, Color(255, 150, 150, 150));

                User& u = userMap[currentLoggedInUserEmail];

                if (currentState == STATE_DASHBOARD) {
                    DrawString(g, "Welcome back, " + u.name + "!", 260, 110, 24, Color(mainAlpha, 255, 255, 255), true);
                    DrawString(g, "Current Balance: Rs. " + to_string((int)u.balance), 260, 150, 16, Color(mainAlpha, 0, 255, 150));
                    
                    // Input Labels
                    DrawString(g, "Pickup Location:", 260, 202, 12, Color(mainAlpha, 220, 220, 220));
                    DrawString(g, "Drop-off Location:", 260, 242, 12, Color(mainAlpha, 220, 220, 220));
                    DrawString(g, "Ride Type Selection:", 260, 282, 12, Color(mainAlpha, 220, 220, 220));
                    DrawString(g, "Counter Offer (Rs):", 570, 262, 12, Color(mainAlpha, 220, 220, 220));

                    // Route & Status Output Area
                    SolidBrush outputBrush(Color(255, 20, 20, 30));
                    g.FillRectangle(&outputBrush, 260, 370, 500, 200);

                    if (routeCalculated) {
                        DrawString(g, "Path:", 280, 385, 12, Color(mainAlpha, 0, 255, 255), true);
                        string pathStr = "";
                        for (size_t i = 0; i < currentRoutePath.size(); ++i) {
                            pathStr += currentRoutePath[i] + (i == currentRoutePath.size() - 1 ? "" : " -> ");
                        }
                        DrawString(g, pathStr, 330, 385, 12, Color(mainAlpha, 255, 255, 255));
                        
                        string infoStr = "Distance: " + to_string(currentRouteDistance) + " km  |  Recommended: Rs. " + to_string(currentRouteFare);
                        DrawString(g, infoStr, 280, 420, 13, Color(mainAlpha, 255, 255, 100), true);
                    }

                    if (!bookingStatus.empty()) {
                        Color statusColor = Color(mainAlpha, 255, 255, 255);
                        if (bookingStatus.find("Success") != string::npos) statusColor = Color(mainAlpha, 0, 255, 150);
                        else if (bookingStatus.find("Error") != string::npos) statusColor = Color(mainAlpha, 255, 100, 100);
                        DrawString(g, bookingStatus, 280, 465, 12, statusColor);
                    }
                }
                else if (currentState == STATE_HISTORY) {
                    DrawString(g, "Your Ride History", 260, 110, 24, Color(mainAlpha, 255, 255, 255), true);
                    SolidBrush listBrush(Color(255, 30, 30, 45));
                    g.FillRectangle(&listBrush, 260, 170, 500, 380);
                    
                    int yPos = 190;
                    if (u.rideHistory.rides.empty()) {
                        DrawString(g, "No rides found in memory.", 380, 350, 14, Color(mainAlpha, 150, 150, 150));
                    } else {
                        // Display last 6 rides to fit in screen nicely
                        int startIdx = max(0, (int)u.rideHistory.rides.size() - 6);
                        for (int i = (int)u.rideHistory.rides.size() - 1; i >= startIdx; --i) {
                            const Ride& r = u.rideHistory.rides[i];
                            string text = r.pickup + " -> " + r.drop + " | Rs. " + to_string(r.fare) + " (" + r.rideType + ")";
                            DrawString(g, text, 280, yPos, 13, Color(255, 220, 220, 220), true);
                            string text2 = "Driver: " + r.driverName + " | Distance: " + to_string(r.distance) + " km";
                            DrawString(g, text2, 280, yPos + 18, 11, Color(255, 0, 255, 255));
                            yPos += 55;
                        }
                    }
                }
                else if (currentState == STATE_WALLET) {
                    DrawString(g, "Digital Wallet", 260, 110, 24, Color(mainAlpha, 255, 255, 255), true);
                    DrawString(g, "Balance: Rs. " + to_string((int)u.balance), 260, 170, 20, Color(mainAlpha, 0, 255, 150), true);
                    
                    DrawString(g, "Enter Amount (Rs.):", 260, 222, 12, Color(mainAlpha, 220, 220, 220));
                }
                else if (currentState == STATE_MAP) {
                    DrawString(g, "City Zones & Locations", 260, 110, 24, Color(mainAlpha, 255, 255, 255), true);
                    SolidBrush listBrush(Color(255, 30, 30, 45));
                    g.FillRectangle(&listBrush, 260, 170, 500, 380);
                    
                    vector<string> zones = getAllZones();
                    int xOffset = 280;
                    int yOffset = 190;
                    int colWidth = 150;
                    int rowHeight = 25;
                    
                    for (size_t i = 0; i < zones.size(); ++i) {
                        DrawString(g, "📍 " + zones[i], xOffset, yOffset, 12, Color(255, 200, 200, 200));
                        yOffset += rowHeight;
                        if (yOffset > 520) {
                            yOffset = 190;
                            xOffset += colWidth;
                        }
                    }
                }
                else if (currentState == STATE_ADMIN) {
                    if (!adminUnlocked) {
                        DrawString(g, "Admin Access Required", 260, 110, 24, Color(mainAlpha, 255, 255, 255), true);
                        SolidBrush cardBrush(Color(200, 30, 30, 50));
                        g.FillRectangle(&cardBrush, 250, 180, 300, 200);
                        
                        DrawString(g, "Enter Admin Password:", 270, 222, 12, Color(255, 220, 220, 220));
                    } else {
                        DrawString(g, "Admin Control Panel", 260, 60, 24, Color(mainAlpha, 255, 255, 255), true);
                        SolidBrush listBrush(Color(255, 30, 30, 45));
                        g.FillRectangle(&listBrush, 260, 150, 500, 400);

                        if (adminSubState == 0) { // View Users
                            DrawString(g, "Name", 280, 170, 12, Color(255, 0, 255, 255), true);
                            DrawString(g, "Email", 430, 170, 12, Color(255, 0, 255, 255), true);
                            DrawString(g, "Wallet", 630, 170, 12, Color(255, 0, 255, 255), true);
                            
                            int yPos = 200;
                            int count = 0;
                            for (auto const &entry : userMap) {
                                if (count++ >= 8) break;
                                const User& usr = entry.second;
                                DrawString(g, usr.name, 280, yPos, 11, Color(255, 220, 220, 220));
                                DrawString(g, usr.email, 430, yPos, 11, Color(255, 220, 220, 220));
                                DrawString(g, "Rs. " + to_string((int)usr.balance), 630, yPos, 11, Color(255, 0, 255, 150));
                                yPos += 35;
                            }
                        }
                        else if (adminSubState == 1) { // View Drivers
                            DrawString(g, "Name", 280, 170, 12, Color(255, 0, 255, 255), true);
                            DrawString(g, "Location", 450, 170, 12, Color(255, 0, 255, 255), true);
                            DrawString(g, "Rating", 630, 170, 12, Color(255, 0, 255, 255), true);
                            
                            int yPos = 200;
                            int count = 0;
                            for (auto const &entry : driverInfoMap) {
                                if (count++ >= 8) break;
                                const Driver& drv = entry.second;
                                DrawString(g, drv.name, 280, yPos, 11, Color(255, 220, 220, 220));
                                DrawString(g, drv.location, 450, yPos, 11, Color(255, 220, 220, 220));
                                DrawString(g, to_string(drv.rating).substr(0, 3) + " ⭐", 630, yPos, 11, Color(255, 255, 255, 100));
                                yPos += 35;
                            }
                        }
                        else if (adminSubState == 2) { // Add Driver Form Labels
                            DrawString(g, "Driver Name:", 270, 162, 12, Color(255, 220, 220, 220));
                            DrawString(g, "Location (Zone):", 270, 202, 12, Color(255, 220, 220, 220));
                            DrawString(g, "Vehicle Model:", 270, 242, 12, Color(255, 220, 220, 220));
                            DrawString(g, "Plate Number:", 270, 282, 12, Color(255, 220, 220, 220));
                            DrawString(g, "Vehicle Type:", 270, 322, 12, Color(255, 220, 220, 220));
                        }
                        else if (adminSubState == 3) { // Analytics
                            DrawString(g, "Popular Route", 280, 170, 12, Color(255, 0, 255, 255), true);
                            DrawString(g, "Rides Completed", 580, 170, 12, Color(255, 0, 255, 255), true);
                            
                            int yPos = 200;
                            if (popularRoutes.empty()) {
                                DrawString(g, "No analytics data recorded yet.", 320, 300, 14, Color(255, 150, 150, 150));
                            } else {
                                int count = 0;
                                for (auto const &pair : popularRoutes) {
                                    if (count++ >= 8) break;
                                    DrawString(g, "📍 " + pair.first, 280, yPos, 11, Color(255, 220, 220, 220));
                                    DrawString(g, to_string(pair.second) + " rides", 580, yPos, 11, Color(255, 0, 255, 150));
                                    yPos += 35;
                                }
                            }
                        }
                    }
                }
            }
            
            BitBlt(hdc, 0, 0, 800, 600, memDC, 0, 0, SRCCOPY);
            DeleteObject(memBitmap);
            DeleteDC(memDC);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY: { PostQuitMessage(0); return 0; }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hI, HINSTANCE hP, LPSTR lp, int nS) {
    GdiplusStartupInput gsi; ULONG_PTR gT; GdiplusStartup(&gT, &gsi, NULL);
    buildGraph(); loadData();
    WNDCLASS wc = {0}; wc.lpfnWndProc = WindowProc; wc.hInstance = hI;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); wc.lpszClassName = "RideApp";
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    RegisterClass(&wc);
    hMainWnd = CreateWindowEx(0, "RideApp", "Ride Booking - Premium GUI", 
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hI, NULL);
    ShowWindow(hMainWnd, nS);
    MSG msg; while (GetMessage(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }
    GdiplusShutdown(gT); return 0;
}
