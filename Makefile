all:
	gcc calculator.h calculator.c main.c -o calculator

run:
	./calculator

test:
	gcc calculator.h calculator.c test.c -o test

