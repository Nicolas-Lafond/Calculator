all:
	gcc calculator.h calculator.c main.c -o calculator

clean:
	rm calculator test

run:
	./calculator

test:
	gcc -g calculator.h calculator.c test.c -o test

