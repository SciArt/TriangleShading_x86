CC = g++
CFLAGS = -Wall -m64
APPNAME = app

all: main.o x86_function.o
	$(CC) $(CFLAGS) -o $(APPNAME) main.o x86_function.o -lsfml-graphics -lsfml-window -lsfml-system

x86_function.o: x86_function.s
	nasm -f elf64 -o x86_function.o x86_function.s

main.o: main.cpp
	$(CC) $(CFLAGS) -c -o main.o main.cpp

clean:
	rm -f *.o

