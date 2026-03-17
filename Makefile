CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -Iinclude -O2

SRC = src/RingBuffer.c
TEST = tests/RingBuffer_test.c

ifeq ($(OS),Windows_NT)
    TARGET = RingBuffer_test.exe
    CLEAN = cmd /C del /F /Q
else
    TARGET = RingBuffer_test
    CLEAN = rm -f
endif

all:
	$(CC) $(CFLAGS) $(SRC) $(TEST) -o $(TARGET)

run: all
	./$(TARGET)

clean:
	$(CLEAN) $(TARGET)

rebuild: clean all