CFLAGS := -Wall
CC := gcc
TARGET := escalona

SRC := $(wildcard *.c)
OBJ := $(addsuffix .o, $(notdir $(basename $(SRC))))

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c grafos.h
	$(CC) -c -o $@ $< $(CCFLAGS)

clean:
	rm $(OBJ)

purge: clean
	rm $(TARGET)
