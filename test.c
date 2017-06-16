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
}

int main(int argc, char* argv[])
{
    test_tokens();
    //test_numbers();
    //test_add();
    //test_to_digit();
}
