CC = gcc
TARGET = testlib
OBJECTS = bitmap.o hash.o list.o main.o hex_dump.o

all = $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^

clean:
	rm -f $(TARGET) $(OBJECTS)
