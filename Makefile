CC=$(PREFIX)gcc

CFLAGS = -Wall -Werror=implicit-function-declaration
CFLAGS += -I.
CFLAGS += -g -DDEBUG

TARGET=bin/ec

build: $(TARGET)

$(TARGET): src/main.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)

.PHONY: build clean

# tests

test-ex: $(TARGET)
	./$(TARGET) testcase/example_A.bench testcase/example_B.bench example.dimacs

test-c17: $(TARGET)
	./$(TARGET) testcase/c17_A.bench testcase/c17_B.bench c17.dimacs
