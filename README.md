# Ride Booking System (DSA Project)

A console-based ride-booking application implemented in C++ using fundamental Data Structures and Algorithms. The system allows users to sign up, log in, and book rides across various locations in a simulated city (Islamabad).

## 🚀 Features

- **User Management**: Secure sign-up and login system for users.
- **Dynamic City Graph**: A graph-based representation of city zones and their connections.
- **Shortest Path Calculation**: Uses **Dijkstra's Algorithm** to find the most efficient route by distance or time.
- **Driver Selection**: Intelligent driver assignment based on distance, rating, and vehicle type.
- **Fare Estimation**: Dynamic fare calculation based on ride type (Bike, Mini, Business Class) and distance.
- **Ride History**: Persistent tracking of user ride history during the session.
- **Zone Search**: Alphabetical sorting and searching of city zones using a **Binary Search Tree (BST)**.

## 🛠️ Data Structures Used

| Data Structure | Purpose |
| :--- | :--- |
| **Graph (Adjacency List)** | Representing city locations and distances. |
| **Dijkstra's Algorithm** | Calculating the shortest path between locations. |
| **Binary Search Tree (BST)** | Efficiently storing and searching city zones. |
| **Priority Queue (Heap)** | Ranking drivers based on proximity and ratings. |
| **Unordered Map (Hash Table)** | Fast lookup for user profiles, drivers, and zone names. |
| **Linked List / Vector** | Storing ride history and dynamic driver lists. |

## 📍 Locations Covered
The system includes major sectors of Islamabad, such as:
- F6, F7, F8, F9, F10
- G6, G7, G8, G9, G10, G11
- Blue Area, Zero Point, Faisal Mosque
- Universities: FAST, NUST, QAU, Air University, COMSATS
- And more...

## 💻 Tech Stack
- **Language**: C++
- **Paradigm**: Object-Oriented Programming (OOP)
- **Environment**: Console/Terminal

## 🏃 How to Run

1.  **Clone the repository**:
    ```bash
    git clone https://github.com/bilalchannar/Ride-Booking.git
    ```
2.  **Compile the code**:
    Use any C++ compiler (like `g++`):
    ```bash
    g++ "Project Code.cpp" -o RideBookingApp
    ```
3.  **Run the executable**:
    ```bash
    ./RideBookingApp
    ```

## 📝 License
This project is for educational purposes. Feel free to use and modify it!

---
Developed as a Data Structures and Algorithms (DSA) Semester Project.
