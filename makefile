# Makefile for Snake Game
CC = gcc
#CFLAGS = -Wall -Wextra -std=c99
CFLAGS = -Wall 
LDFLAGS = -lncurses
TARGET = lependu score.hi
SRC = lependu.c
OBJ = $(SRC:.c=.o)
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f $(OBJ)

mrproper: clean
	rm -f $(TARGET)

re: mrproper all

.PHONY: all clean mrproper

dependencie:
	lependu.o:lependu.c pendu.h
