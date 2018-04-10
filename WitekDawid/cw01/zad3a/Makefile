CC = gcc -Wall -std=c11 -ggdb

all: static shared

static:
	$(CC) -c library.c
	ar rcs library.a library.o

shared:
	$(CC) -c -fPIC library.c
	$(CC) -shared -fPIC -o library.so library.o

clean:
	rm -f *.o

zad3a_static:
	make static
	$(CC) -o test main.c library.a -I . -O$(0level)
	make tests

zad3a_shared:
	make shared
	$(CC) -o test main.c -L. -Wl,-rpath=. library.so -I . -O$(0level)
	make tests

zad3a_dynamic:
	make shared
	$(CC) -o test main_dynamic.c -I . -O$(0level) -ldl
	make tests

tests:
	./test static 250000 140 change_obo 70000 change_all 700000 | tee -a results3a.txt
	./test static 250000 180 change_all 70000 remove | tee -a results3a.txt
	./test static 250000 300 find_nearest 50000 change_obo 50000 | tee -a results3a.txt

	./test dynamic 250000 140 change_obo 70000 change_all 70000 | tee -a results3a.txt
	./test dynamic 250000 180 change_all 70000 remove | tee -a results3a.txt
	./test dynamic 250000 300 find_nearest 50000 change_obo 50000 | tee -a results3a.txt
