CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O3
TARGET = bin/ec

OBJS = src/parse_bench.o src/miter.o src/main.o

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Pattern rule to build .o from .cpp
src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) src/*.o

.PHONY: all clean