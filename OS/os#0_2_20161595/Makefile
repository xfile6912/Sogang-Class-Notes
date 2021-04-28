testlib : bitmap.o debug.o hash.o hex_dump.o list.o main.o
	gcc -o testlib bitmap.o debug.o hash.o hex_dump.o list.o main.o

bitmap.o : bitmap.c
	gcc -c -o bitmap.o bitmap.c

debug.o : debug.c
	gcc -c -o debug.o debug.c

hash.o : hash.c
	gcc -c -o hash.o hash.c

hex_dump.o : hex_dump.c
	gcc -c -o hex_dump.o hex_dump.c

list.o : list.c
	gcc -c -o list.o list.c

main.o : main.c
	gcc -c -o main.o main.c

clean:
	rm *.o testlib
