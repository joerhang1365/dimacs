CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O3
TARGET = bin/ec

OBJS =  \
	src/bench.o \
	src/miter.o \
	src/cnf.o \
	src/main.o \

build: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Pattern rule to build .o from .cpp
src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) src/*.o

.PHONY: build clean

test-ex:
	./bin/ec testcase/example_A.bench testcase/example_B.bench example.dimacs