/**
 * @file jarvis_client.cpp
 * @brief Client for Convex Hull Server using Jarvis March algorithm
 * @author Your Name
 * @date Creation Date
 */

 #include <iostream>
 #include <string>
 #include <vector>
 #include <sstream>
 #include <stdexcept>
 #include <winsock2.h>
 #include <ws2tcpip.h>
 #include "jarvis_hull.h"
 
 namespace {
     constexpr const char* SERVER_IP = "127.0.0.1"; ///< Server IP address
     constexpr int JARVIS_PORT = 9090; ///< Server port number
     constexpr int BUFFER_SIZE = 4096; ///< Network buffer size
     constexpr int TIMEOUT_MS = 5000; ///< Connection timeout in milliseconds
 }
 
 /**
  * @class NetworkClient
  * @brief Handles network communication with the server
  */
 class NetworkClient {
 public:
     /**
      * @brief Constructor initializes Winsock and creates socket
      * @throws std::runtime_error if initialization fails
      */
     NetworkClient() {
         if (WSAStartup(MAKEWORD(2, 2), &wsaData_) != 0) {
             throw std::runtime_error("WSAStartup failed");
         }
 
         socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
         if (socket_ == INVALID_SOCKET) {
             WSACleanup();
             throw std::runtime_error("Socket creation failed");
         }
 
         // Set receive timeout
         setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, 
                   (const char*)&TIMEOUT_MS, sizeof(TIMEOUT_MS));
     }
 
     /// Destructor cleans up socket and Winsock
     ~NetworkClient() {
         if (socket_ != INVALID_SOCKET) closesocket(socket_);
         WSACleanup();
     }
 
     /**
      * @brief Connects to the server
      * @throws std::runtime_error if connection fails
      */
     void connectToServer() {
         sockaddr_in serverAddr = {};
         serverAddr.sin_family = AF_INET;
         serverAddr.sin_port = htons(JARVIS_PORT);
         serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
 
         if (connect(socket_, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
             throw std::runtime_error("Connection to server failed");
         }
     }
 
     /**
      * @brief Sends request and receives response from server
      * @param data Data string to send
      * @return Server response as string
      * @throws std::runtime_error if communication fails
      */
     std::string sendRequest(const std::string& data) {
         // Send data
         if (send(socket_, data.c_str(), data.size(), 0) == SOCKET_ERROR) {
             throw std::runtime_error("Data sending failed");
         }
 
         // Receive response
         char buffer[BUFFER_SIZE];
         std::string response;
         int bytesReceived;
 
         while ((bytesReceived = recv(socket_, buffer, BUFFER_SIZE - 1, 0)) > 0) {
             buffer[bytesReceived] = '\0';
             response += buffer;
         }
 
         if (bytesReceived == SOCKET_ERROR) {
             throw std::runtime_error("Error receiving data");
         }
 
         return response;
     }
 
 private:
     SOCKET socket_ = INVALID_SOCKET; ///< Network socket handle
     WSADATA wsaData_; ///< Winsock data
 };
 
 /**
  * @class InputHandler
  * @brief Handles user input parsing and validation
  */
 class InputHandler {
 public:
     /**
      * @brief Parses input string into vector of HullPoints
      * @param input String in format "x,y x,y ..."
      * @return Vector of parsed points
      * @throws std::invalid_argument if input is invalid
      */
     static std::vector<HullPoint> parseInput(const std::string& input) {
         std::vector<HullPoint> points;
         std::istringstream iss(input);
         std::string token;
 
         while (std::getline(iss, token, ' ')) {
             if (token.empty()) continue;
 
             size_t commaPos = token.find(',');
             if (commaPos == std::string::npos) {
                 throw std::invalid_argument("Invalid point format. Use 'x,y'");
             }
 
             try {
                 int x = std::stoi(token.substr(0, commaPos));
                 int y = std::stoi(token.substr(commaPos + 1));
                 points.push_back({x, y});
             } catch (...) {
                 throw std::invalid_argument("Invalid coordinates");
             }
         }
 
         if (points.size() < 3) {
             throw std::invalid_argument("At least 3 points required");
         }
 
         return points;
     }
 };
 
 /**
  * @brief Main program entry point
  * @return EXIT_SUCCESS on success, EXIT_FAILURE on error
  */
 int main() {
     try {
         // Initialize network client
         NetworkClient client;
         client.connectToServer();
 
         // Get user input
         std::cout << "Enter points (format 'x,y x,y ...'): ";
         std::string userInput;
         std::getline(std::cin, userInput);
 
         // Parse and validate input
         auto points = InputHandler::parseInput(userInput);
 
         // Calculate local convex hull
         auto localHull = computeJarvisHull(points);
         std::string localResult = formatHull(localHull);
 
         // Send request to server
         std::string request = formatHull(points);
         std::string serverResponse = client.sendRequest(request);
 
         // Display results
         std::cout << "\nServer response: " << serverResponse << std::endl;
         std::cout << "Local result:    " << localResult << std::endl;
 
         if (serverResponse == localResult) {
             std::cout << "\nResults match!" << std::endl;
         } else {
             std::cout << "\nWarning: Results don't match!" << std::endl;
         }
 
     } catch (const std::exception& e) {
         std::cerr << "Error: " << e.what() << std::endl;
         return EXIT_FAILURE;
     }
 
     return EXIT_SUCCESS;
 }