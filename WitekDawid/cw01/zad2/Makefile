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

zad2:
	make static
	$(CC) -o test main.c library.a -I . -O$(0level)
	make tests

tests:
	./test static 100 32 | tee -a raport2.txt
	./test static 250000 140 change_obo 70000 change_all 700000 | tee -a raport2.txt
	./test static 250000 180 change_all 70000 remove | tee -a raport2.txt
	./test static 250000 160 find_nearest 5000 change_obo 50000 | tee -a raport2.txt
	./test static 250000 300 change_all 140000 remove | tee -a raport2.txt

	./test dynamic 100 32 | tee -a raport2.txt
	./test dynamic 250000 140 change_obo 70000 change_all 70000 | tee -a raport2.txt
	./test dynamic 250000 180 change_all 70000 remove | tee -a raport2.txt
	./test dynamic 250000 160 find_nearest 5000 change_obo 50000 | tee -a raport2.txt
	./test dynamic 250000 300 change_all 140000 remove | tee -a raport2.txt
