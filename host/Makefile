# Compiler and flags
CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I..
LDFLAGS  = -lraylib -lenet -lGL -lm -lpthread -ldl -lrt -lX11

# Source files
# Add new .cpp files here (e.g. src/Player.cpp src/Network.cpp)
# Source files
SRCS = main.cpp ../game.cpp ../player.cpp ../collisions.cpp ../world.cpp ../protocol.cpp ../attack.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)
DEPS   = $(OBJS:.o=.d)
# Output binary
TARGET = game

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
