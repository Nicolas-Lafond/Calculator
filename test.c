#include <assert.h>
#include "calculator.h"

void test_tokens()
{
    Token tok;

    while(tok.type != CARRIAGE_RETURN) {
        tok = next_token();
        print_token(&tok);
        printf("\n");
    }
}

void test_create_number_from_int()
{
    Number *num;

    num = create_number_from_int(12345);
    print_number(num);
    printf("\n");

    num = create_number_from_int(7424);
    print_number(num);
    printf("\n");

    num = create_number_from_int(4);
    print_number(num);
    printf("\n");
}

void test_number_of_digits()
{
    assert(number_of_digits(123) == 3);
    assert(number_of_digits(3245) == 4);
    assert(number_of_digits(2) == 1);
    assert(number_of_digits(1) == 1);
    assert(number_of_digits(0) == 1);
    assert(number_of_digits(10000) == 5);
    assert(number_of_digits(10) == 2);
    assert(number_of_digits(-3) == 1);
}

void test_numbers()
{
    Number *num;
    Digit *digit;

    num = (Number*) malloc(sizeof(Number));
    num->sign = POSITIVE;
    num->next = NULL;

    digit = (Digit*) malloc(sizeof(Digit));
    num->last = digit;
    digit->previous = NULL;
    digit->value = 3;
    digit->next = (Digit*) malloc(sizeof(Digit));
    digit->next->previous = digit;
    digit = digit->next;
    digit->value = 2;
    digit->next = (Digit*) malloc(sizeof(Digit));
    digit->next->previous = digit;
    digit = digit->next;
    digit->value = 1;
    digit->next = NULL;

    //should print 123
    print_number(num);
    printf("\n");

    //should print 1 followed by the number enter in standard input
    num = _read_number('1');
    print_number(num);
    printf("\n");

}

void test_to_digit()
{
    assert(to_digit('0') == 0);
    assert(to_digit('1') == 1);
    assert(to_digit('2') == 2);
    assert(to_digit('3') == 3);
    assert(to_digit('4') == 4);
    assert(to_digit('5') == 5);
    assert(to_digit('6') == 6);
    assert(to_digit('7') == 7);
    assert(to_digit('8') == 8);
    assert(to_digit('9') == 9);

    assert(to_digit('a') == -1);
    assert(to_digit('A') == -1);
    assert(to_digit('/') == -1);
    assert(to_digit('&') == -1);
}

void test_add()
{
    Number *num1, *num2, *sum;

    num1 = create_number_from_int(123);
    num2 = create_number_from_int(832);
    sum = _add(num1, num2);
    print_number(sum);
    printf("\n");

    num1 = create_number_from_int(28429);
    num2 = create_number_from_int(82421132);
    sum = _add(num1, num2);
    print_number(sum);
    printf("\n");

    num1 = create_number_from_int(24);
    num2 = create_number_from_int(82421132);
    sum = _add(num1, num2);
    print_number(sum);
    printf("\n");

    num1 = create_number_from_int(2);
    num2 = create_number_from_int(8);
    sum = _add(num1, num2);
    print_number(sum);
    printf("\n");

    num1 = create_number_from_int(284);
    num2 = create_number_from_int(824);
    sum = _add(num1, num2);
    print_number(sum);
    printf("\n");

    num1 = create_number_from_int(28429);
    num2 = create_number_from_int(82);
    sum = _add(num1, num2);
    print_number(sum);
    printf("\n");
}

int main(int argc, char* argv[])
{
    //test_tokens();
    //test_numbers();
    test_add();
    //test_to_digit();
    //test_create_number_from_int();
    //test_number_of_digits();
}
