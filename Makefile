CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I include

SRC      = src/can_config.cpp \
		   src/can_sender.cpp \
		   src/can_reader.cpp \
           src/main.cpp

OBJ      = $(SRC:.cpp=.o)
TARGET   = can_logger

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
