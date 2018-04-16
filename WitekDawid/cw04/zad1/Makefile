MAIN = main.o
MAINEXE = main
MAIN2 = main2.o
MAINEXE2 = main2

all: $(MAIN) $(MAINEXE) $(MAIN2) $(MAINEXE2)

clean:
	rm -r $(MAIN) $(MAINEXE) $(MAIN2) $(MAINEXE2)

$(MAIN): main.c
	gcc -c main.c

$(MAINEXE): $(MAIN)
	gcc $(MAIN) -o $(MAINEXE)

$(MAIN2): main2.c
	gcc -c main2.c

$(MAINEXE2): $(MAIN2)
	gcc $(MAIN2) -o $(MAINEXE2)