CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g
TARGET = bin/ec

OBJS =  \
	src/bench.o \
	src/miter.o \
	src/cnf.o \
	src/main.o \

build: bin $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# build .o from .cpp
src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

bin:
	mkdir -p bin

clean:
	rm -rf src/*.o *.dimacs *.output bin

test-ex:
	./bin/ec testcase/example_A.bench testcase/example_B.bench example.dimacs

test-c17:
	./bin/ec testcase/c17_A.bench testcase/c17_B.bench c17.dimacs

test-c432:
	./bin/ec testcase/c432_A.bench testcase/c432_B.bench c432.dimacs

.PHONY: build bin clean