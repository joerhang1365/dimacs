CC=$(PREFIX)gcc

CFLAGS = -Wall -Werror=implicit-function-declaration
CFLAGS += -I.

TARGET=bin/ec

build: $(TARGET)

$(TARGET): src/main.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)

.PHONY: build clean
