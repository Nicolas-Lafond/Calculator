#include <assert.h>
#include "calculator.h"

void test_tokens()
{
    char *str = "12 4 65 + - *";
    Token *tok;

    *tok = next_token();
    print_token(tok);
    *tok = next_token();
    print_token(tok);
    *tok = next_token();
    print_token(tok);
    *tok = next_token();
    print_token(tok);
    *tok = next_token();
    print_token(tok);
}

void test_numbers()
{
    Number *num;
    Digit *digit;

    num = malloc(sizeof(Number));
    num->sign = POSITIVE;
    num->next = NULL;

    digit = malloc(sizeof(Digit));
    digit->previous = NULL;
    digit->value = 3;
    digit->next = malloc(sizeof(Digit));
    digit->next->previous = digit;
    digit = digit->next;
    digit->value = 2;
    digit->next = malloc(sizeof(Digit));
    digit->next->previous = digit;
    digit = digit->next;
    digit->value = 1;
    digit->next = NULL;

    //should print 321
    print_number(num);

}

void test_add()
{
}

int main(int argc, char* argv[])
{
    //test_tokens();
    test_numbers();
    //test_add();
}
