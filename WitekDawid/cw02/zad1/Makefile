CC = gcc -Wall -std=c11 -ggdb

all: compile

compile:
	$(CC) -o test main.c
	make tests

clean:
	rm -f dane plik plik2 test

clean2:
	rm -f wyniki.txt

tests:
	./test generate dane 1000 4	| tee -a wyniki.txt
	./test copy dane plik 1000 4 lib | tee -a wyniki.txt
	./test copy dane plik2 1000 4 sys | tee -a wyniki.txt
	./test sort plik 1000 4 lib | tee -a wyniki.txt
	./test sort plik2 1000 4 sys | tee -a wyniki.txt

	./test generate dane 10000 4 | tee -a wyniki.txt
	./test copy dane plik 10000 4 lib | tee -a wyniki.txt
	./test copy dane plik2 10000 4 sys | tee -a wyniki.txt
	./test sort plik 10000 4 lib | tee -a wyniki.txt
	./test sort plik2 10000 4 sys | tee -a wyniki.txt

	./test generate dane 1000 512 | tee -a wyniki.txt
	./test copy dane plik 1000 512 lib | tee -a wyniki.txt
	./test copy dane plik2 1000 512 sys | tee -a wyniki.txt
	./test sort plik 1000 512 lib | tee -a wyniki.txt
	./test sort plik2 1000 512 sys | tee -a wyniki.txt

	./test generate dane 10000 512 | tee -a wyniki.txt
	./test copy dane plik 10000 512 lib | tee -a wyniki.txt
	./test copy dane plik2 10000 512 sys | tee -a wyniki.txt
	./test sort plik 10000 512 lib | tee -a wyniki.txt
	./test sort plik2 10000 512 sys | tee -a wyniki.txt

	./test generate dane 1000 4096 | tee -a wyniki.txt
	./test copy dane plik 1000 4096 lib | tee -a wyniki.txt
	./test copy dane plik2 1000 4096 sys | tee -a wyniki.txt
	./test sort plik 1000 4096 lib | tee -a wyniki.txt
	./test sort plik2 1000 4096 sys | tee -a wyniki.txt

	./test generate dane 10000 4096 | tee -a wyniki.txt
	./test copy dane plik 10000 4096 lib | tee -a wyniki.txt
	./test copy dane plik2 10000 4096 sys | tee -a wyniki.txt
	./test sort plik 10000 4096 lib | tee -a wyniki.txt
	./test sort plik2 10000 4096 sys | tee -a wyniki.txt

	./test generate dane 1000 8192 | tee -a wyniki.txt
	./test copy dane plik 1000 8192 lib | tee -a wyniki.txt
	./test copy dane plik2 1000 8192 sys | tee -a wyniki.txt
	./test sort plik 1000 8192 lib | tee -a wyniki.txt
	./test sort plik2 1000 8192 sys | tee -a wyniki.txt

	./test generate dane 10000 8192 | tee -a wyniki.txt
	./test copy dane plik 10000 8192 lib | tee -a wyniki.txt
	./test copy dane plik2 10000 8192 sys | tee -a wyniki.txt
	./test sort plik 10000 8192 lib | tee -a wyniki.txt
	./test sort plik2 10000 8192 sys | tee -a wyniki.txt