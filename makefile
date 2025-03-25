CC = gcc
CFLAGS = -Wall -ansi -pedantic
SOURCES = assembler.c pre_assembler.c first_pass.c second_pass.c syntax_errors.c syntax_check.c string_operations.c names.c memory.c cell_operations.c main.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = my_assembler

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)

%.o: %.c header.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
