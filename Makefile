# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -I.

# Исходные файлы
CLIENT_SRC = jarvis_client.cpp
SERVER_SRC = jarvis_server.cpp
TEST_SRC = jarvis_mass_test.cpp
HEADER = jarvis_hull.h

# Исполняемые файлы
CLIENT_EXE = jarvis_client
SERVER_EXE = jarvis_server
TEST_EXE = jarvis_test

# Библиотеки для Windows
WIN_LIBS = -lws2_32

all: $(CLIENT_EXE) $(SERVER_EXE) $(TEST_EXE)

$(CLIENT_EXE): $(CLIENT_SRC) $(HEADER)
	$(CXX) $(CXXFLAGS) -o $@ $< $(WIN_LIBS)

$(SERVER_EXE): $(SERVER_SRC) $(HEADER)
	$(CXX) $(CXXFLAGS) -o $@ $< $(WIN_LIBS)

$(TEST_EXE): $(TEST_SRC) $(HEADER)
	$(CXX) $(CXXFLAGS) -o $@ $< $(WIN_LIBS)

clean:
	rm -f $(CLIENT_EXE) $(SERVER_EXE) $(TEST_EXE) *.o

.PHONY: all clean