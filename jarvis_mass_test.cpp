/**
 * @file jarvis_mass_test.cpp
 * @brief Массовое тестирование сервера выпуклой оболочки (Jarvis March)
 */

 #include <iostream>
 #include <fstream>
 #include <winsock2.h>
 #include <vector>
 #include <random>
 #include <sstream>
 #include <unordered_set>
 #include <algorithm>
 #include "jarvis_hull.h"
 
 #define PORT 9090
 #define BUFFER_SIZE 4096
 #define NUM_CLIENTS 100
 #define POINTS_PER_TEST 50
 #define TEST_LOG_FILE "jarvis_tests.log"
 #define MIN_COORD -1000
 #define MAX_COORD 1000
 
 // Генерация уникальных точек
 std::vector<HullPoint> generateUniquePoints(int count, std::mt19937& gen, std::uniform_int_distribution<>& dist) {
     std::vector<HullPoint> points;
     std::unordered_set<std::string> point_set;
     
     while (points.size() < static_cast<size_t>(count)) {
         HullPoint p{dist(gen), dist(gen)};
         std::string key = std::to_string(p.x) + "," + std::to_string(p.y);
         
         if (point_set.find(key) == point_set.end()) {
             point_set.insert(key);
             points.push_back(p);
         }
     }
     return points;
 }
 
 // Сохранение теста в файл
 void saveTestToFile(const std::vector<HullPoint>& points, const std::vector<HullPoint>& hull) {
     std::ofstream out(TEST_LOG_FILE, std::ios::app);
     out << "POINTS:";
     for (const auto& p : points) {
         out << " " << p.x << "," << p.y;
     }
     out << "\nHULL:";
     for (const auto& p : hull) {
         out << " " << p.x << "," << p.y;
     }
     out << "\n---\n";
 }
 
 // Функция тестирования клиента
 bool testClient(const std::vector<HullPoint>& testPoints, int clientId) {
     WSADATA wsaData;
     if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
         std::cerr << "Client " << clientId << ": WSAStartup failed\n";
         return false;
     }
 
     SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
     if (sock == INVALID_SOCKET) {
         std::cerr << "Client " << clientId << ": Socket error\n";
         WSACleanup();
         return false;
     }
 
     sockaddr_in serv_addr;
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_port = htons(PORT);
     serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Замена inet_pton на inet_addr
 
     if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
         std::cerr << "Client " << clientId << ": Connection failed\n";
         closesocket(sock);
         WSACleanup();
         return false;
     }
 
     std::string input = formatHull(testPoints);
     std::vector<HullPoint> localHull = computeJarvisHull(testPoints);
     saveTestToFile(testPoints, localHull);
 
     if (send(sock, input.c_str(), input.size(), 0) == SOCKET_ERROR) {
         std::cerr << "Client " << clientId << ": Send failed\n";
         closesocket(sock);
         WSACleanup();
         return false;
     }
 
     char buffer[BUFFER_SIZE];
     std::string serverResponse;
     int bytesReceived;
 
     while ((bytesReceived = recv(sock, buffer, BUFFER_SIZE - 1, 0)) > 0) {
         buffer[bytesReceived] = '\0';
         serverResponse += buffer;
     }
 
     bool success = (serverResponse == formatHull(localHull));
     closesocket(sock);
     WSACleanup();
     return success;
 }
 
 int main() {
     std::ofstream(TEST_LOG_FILE, std::ios::trunc).close();
 
     std::random_device rd;
     std::mt19937 gen(rd());
     std::uniform_int_distribution<> dist(MIN_COORD, MAX_COORD);
 
     int passed = 0;
     for (int i = 0; i < NUM_CLIENTS; ++i) {
         auto points = generateUniquePoints(POINTS_PER_TEST, gen, dist);
         if (testClient(points, i)) passed++;
         std::cout << "Test " << i+1 << "/" << NUM_CLIENTS 
                   << " | Passed: " << passed << "\r";
     }
 
     std::cout << "\nTests completed: " << passed << "/" << NUM_CLIENTS 
               << " (" << (passed*100/NUM_CLIENTS) << "%)\n";
     return 0;
 }