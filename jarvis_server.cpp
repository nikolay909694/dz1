/**
 * @file jarvis_server.cpp
 * @brief Server for computing convex hull using Jarvis March algorithm
 * @author Your Name
 * @date Creation Date
 */

 #include <iostream>
 #include <winsock2.h>
 #include <ws2tcpip.h>
 #include <vector>
 #include <sstream>
 #include "jarvis_hull.h"
 
 #define JARVIS_PORT 9090             ///< Server listening port
 #define BUFFER_CAPACITY 2048         ///< Receive buffer size
 #define MAX_PENDING_CONNECTIONS 50   ///< Maximum pending connections queue
 
 /**
  * @brief Parses coordinate string into vector of HullPoints
  * @param data String in format "x,y x,y ..."
  * @return Vector of parsed points
  */
 std::vector<HullPoint> parseCoordinates(const std::string& data) {
     std::vector<HullPoint> result;
     std::istringstream stream(data);
     int x_val, y_val;
     char delimiter;
     
     while (stream >> x_val >> delimiter >> y_val) {
         result.push_back({x_val, y_val});
         if (stream.peek() == ' ') stream.ignore();
     }
     return result;
 }
 
 /**
  * @brief Main server entry point
  * @return 0 on normal exit, 1 on error
  */
 int main() {
     WSADATA wsa;
     if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
         std::cerr << "WSA initialization failed\n";
         return 1;
     }
 
     SOCKET master_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
     if (master_socket == INVALID_SOCKET) {
         std::cerr << "Socket creation error\n";
         WSACleanup();
         return 1;
     }
 
     sockaddr_in server_addr;
     server_addr.sin_family = AF_INET;
     server_addr.sin_addr.s_addr = INADDR_ANY;
     server_addr.sin_port = htons(JARVIS_PORT);
 
     if (bind(master_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
         std::cerr << "Bind failed\n";
         closesocket(master_socket);
         WSACleanup();
         return 1;
     }
 
     if (listen(master_socket, MAX_PENDING_CONNECTIONS) == SOCKET_ERROR) {
         std::cerr << "Listen error\n";
         closesocket(master_socket);
         WSACleanup();
         return 1;
     }
 
     std::cout << "Jarvis March server running on port " << JARVIS_PORT << std::endl;
 
     while (true) {
         sockaddr_in client_addr;
         int addr_len = sizeof(client_addr);
         SOCKET client_socket = accept(master_socket, (sockaddr*)&client_addr, &addr_len);
         
         if (client_socket == INVALID_SOCKET) {
             std::cerr << "Accept error\n";
             continue;
         }
 
         char buffer[BUFFER_CAPACITY];
         int bytes_received = recv(client_socket, buffer, BUFFER_CAPACITY, 0);
         if (bytes_received <= 0) {
             closesocket(client_socket);
             continue;
         }
 
         std::string request(buffer, bytes_received);
         auto points = parseCoordinates(request);
         auto hull = computeJarvisHull(points);
         std::string response = formatHull(hull);
 
         send(client_socket, response.c_str(), response.size(), 0);
         closesocket(client_socket);
     }
 
     closesocket(master_socket);
     WSACleanup();
     return 0;
 }