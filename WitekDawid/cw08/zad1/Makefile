main:
	gcc -Wall -o main main.c -lm -lrt -pthread

clean:
	rm -r main result.ascii.pgm Times.txt

result:
	./main 1 casablanca.ascii.pgm filtr1.txt result.ascii.pgm
	./main 2 casablanca.ascii.pgm filtr1.txt result.ascii.pgm
	./main 4 casablanca.ascii.pgm filtr1.txt result.ascii.pgm
	./main 8 casablanca.ascii.pgm filtr1.txt result.ascii.pgm
	./main 1 casablanca.ascii.pgm filtr2.txt result.ascii.pgm
	./main 2 casablanca.ascii.pgm filtr2.txt result.ascii.pgm
	./main 4 casablanca.ascii.pgm filtr2.txt result.ascii.pgm
	./main 8 casablanca.ascii.pgm filtr2.txt result.ascii.pgm
	./main 1 casablanca.ascii.pgm filtr3.txt result.ascii.pgm
	./main 2 casablanca.ascii.pgm filtr3.txt result.ascii.pgm
	./main 4 casablanca.ascii.pgm filtr3.txt result.ascii.pgm
	./main 8 casablanca.ascii.pgm filtr3.txt result.ascii.pgm
	./main 1 gator.ascii.pgm filtr1.txt result.ascii.pgm
	./main 2 gator.ascii.pgm filtr1.txt result.ascii.pgm
	./main 4 gator.ascii.pgm filtr1.txt result.ascii.pgm
	./main 8 gator.ascii.pgm filtr1.txt result.ascii.pgm
	./main 1 gator.ascii.pgm filtr2.txt result.ascii.pgm
	./main 2 gator.ascii.pgm filtr2.txt result.ascii.pgm
	./main 4 gator.ascii.pgm filtr2.txt result.ascii.pgm
	./main 8 gator.ascii.pgm filtr2.txt result.ascii.pgm
	./main 1 gator.ascii.pgm filtr3.txt result.ascii.pgm
	./main 2 gator.ascii.pgm filtr3.txt result.ascii.pgm
	./main 4 gator.ascii.pgm filtr3.txt result.ascii.pgm
	./main 8 gator.ascii.pgm filtr3.txt result.ascii.pgm