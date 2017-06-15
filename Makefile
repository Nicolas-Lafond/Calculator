all:
	gcc calculator.h calculator.c main.c -o tp1

test:
	gcc test.c -o test
