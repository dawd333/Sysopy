MAIN = main.o
MAINEXE = main

all: $(MAIN) $(MAINEXE)

clean:
	rm -r $(MAIN) $(MAINEXE)
	rm -rf katalog plik.txt

$(MAIN): main.c
	gcc -c main.c

$(MAINEXE): $(MAIN)
	gcc $(MAIN) -o $(MAINEXE)
