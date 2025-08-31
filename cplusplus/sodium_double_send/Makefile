CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wextra -pedantic
LDFLAGS = -lsodium -pthread
TARGET = rsds

SRCS = main.cpp state_machine.cpp thread_safe_fifo.cpp utils.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean

run1:
	./rsds 64000 64001 0.0.0.0
run2:
	./rsds 64001 64000 0.0.0.0
