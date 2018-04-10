MAIN = main.o
MAINEXE = main
MEMORYTEST = memoryTest.o
MEMORYTESTEXE = memoryTest
TIMETEST = timeTest.o
TIMETESTEXE = timeTest

all: $(MAIN) $(MAINEXE) $(MEMORYTEST) $(MEMORYTESTEXE) $(TIMETEST) $(TIMETESTEXE)

clean:
	rm -r $(MAIN) $(MAINEXE) $(MEMORYTEST) $(MEMORYTESTEXE) $(TIMETEST) $(TIMETESTEXE)
	rm -rf katalog plik.txt

$(MAIN): main.c
	gcc -c main.c

$(MAINEXE): $(MAIN)
	gcc $(MAIN) -o $(MAINEXE)

$(MEMORYTEST): memoryTest.c
	gcc -c memoryTest.c

$(MEMORYTESTEXE): $(MEMORYTEST)
	gcc $(MEMORYTEST) -o $(MEMORYTESTEXE)

$(TIMETEST): timeTest.c
	gcc -c timeTest.c

$(TIMETESTEXE): $(TIMETEST)
	gcc $(TIMETEST) -o $(TIMETESTEXE)