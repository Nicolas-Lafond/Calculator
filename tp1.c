/* TP1 IFT2035
 * by Nicolas Lafond 
 * version 2
 * postfix calculator with infinite precision
 * numbers are stocked in linked list where each digit is stocked in an element
 * the first digit in the list is the last digit of the number represented
 * addition, substraction and multiplication are supported but not division
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TRUE 1
#define FALSE 0

// a structure for digits in a number
// the next pointer point to the next more significant digit
// the previous pointer point to the next less significant digit
typedef struct Digit {
    int value;
    struct Digit *next;
    struct Digit *previous;
} Digit;

// Represent a number in the stack
typedef struct Number {
    int sign;  // 0 : negative  1 : positive
    struct Digit *last; // pointer to the least significant digit
    struct Number *next; // pointer to the element below on the stack
    int nb_ref; // number of variables who refere to that number
} Number;

typedef struct Stack {
    struct Number *top; 
    int nb_elem;  
} Stack;


typedef enum token_type {
    NUMBER,
    PLUS,
    MINUS,
    MULTIPLY,
    ASSIGNMENT,
    VARIABLE,
    CARRIAGE_RETURN,
    END_OF_FILE,
    ERROR
} token_type;

typedef struct Token {
    token_type type;
    union {
        Number* num;   // if type == NUMBER
        char variable; // if type == VARIABLE or ASSIGNMENT
    } data;
} Token;


// Stack functions BEGIN
void push(Stack *s, Number *a)
{
    a->next = s->top;
    s->top = a;
    s->nb_elem++;
}

Number* pop(Stack *s)
{
    if (s->nb_elem == 0)
        return NULL;
    else {
        Number *n = s->top;
        s->top = s->top->next;
        s->nb_elem--;
        return n;
    }
}

int exponent(int number, int exp)
{
    int value = 1;
    for (int i = 0; i < exp; i++)
        value *= number;
    return value;
}

Digit* create_digit(int value, Digit *next, Digit *previous)
{
    Digit *digit = malloc(sizeof(Digit));
    digit->value = value;
    digit->next = next;
    digit->previous = previous;
    return digit;
}

// Create a digit that correspond to the digit at position
// given at the position specified
// examble : nth_digit(1234, 1) return 4
Digit* nth_digit(int number, int position)
{

    int digit = (number % (exponent(10, position))) / 
                    (exponent(10, position -1));
    return create_digit(digit, NULL, NULL);

}

// A small function to easily create a number from an int
// mostly for testing purposes, we don't assume that the
// number can be arbitrary long like in the calculator
Number* create_number_from_int(int num)
{
    Number *number = malloc(sizeof(Number));
    if (num < 0)
        number->sign = 0;
    else
        number->sign = 1;

    int position = 0;

    return number;
}

void empty_stack(Stack *s)
{
    Number *num;
    while (s->nb_elem > 0) {
        num = pop(s);
        if (num->nb_ref == 0)
            free(num);
    }
}


Digit* add_digits(Digit *digit1, Digit *digit2, int *excess)
{
    if (digit1 == NULL && digit2 == NULL) {
        *excess = 0;
        return NULL;
    }

    Digit *sum = malloc(sizeof(Digit));
    if ((digit1 == NULL) && (digit2 != NULL)) {
        sum->value = digit2->value + *excess;
    }
    else if ((digit1 != NULL) && (digit2 == NULL)) {
        sum->value = digit1->value + *excess;
    }
    else if ((digit1 != NULL) && (digit2 != NULL)) {
        sum->value = digit1->value + digit2->value + *excess;
    }

    if (sum->value > 9) {
        sum->value -= 10;
        *excess = 1;
    }
    else
        *excess = 0;

    sum->next = NULL;
    sum->previous = NULL;

    return sum;
}

// NOTE: return -1 on error
int add(Stack *stack)
{
    if (stack->nb_elem < 2)
        return -1;

    Number *num1 = stack->top;
    Number *num2 = stack->top->next;
    Number *sum = malloc(sizeof(Number));

    Digit *digit1, *digit2, *digit_sum, *current_digit;
    int excess = 0;

    digit1 = num1->last;
    digit2 = num2->last;

    // this code is a bit ugly, should probably refactor
    digit_sum = add_digits(digit1, digit2, &excess);
    sum->last = digit_sum;
    digit1 = digit1->next;
    digit2 = digit2->next;
    current_digit = digit_sum;

    while ((digit1 != NULL) || (digit2 != NULL)) {
        digit_sum = add_digits(digit1, digit2, &excess);
        digit_sum->previous = current_digit;
        current_digit->next = digit_sum;
        digit1 = digit1->next;
        digit2 = digit2->next;
        current_digit = digit_sum;
    }

    return 0;
}

int sub(Stack *stack)
{
    if (stack->nb_elem < 2)
        return -1;
}

int mul(Stack *stack)
{
    if (stack->nb_elem < 2)
        return -1;
}

void assignment(char variable, Number *num) 
{
}

Number* variable_value(char variable)
{
}

// Stack functions END

Number* create_number()
{
    Number *num = malloc(sizeof(Number));
    num->sign = 1;
    num->last = NULL;
    num->next = NULL;
    num->nb_ref = 0;
}

void delete_number(Number *num)
{
    Digit *digit = num->last;
    Digit *next;
    while (digit != NULL) {
        next = digit->next;
        free(digit);
        digit = next;
    }
    free(num);
}

void print_number(Number *number) {
    if (number == NULL)
        return;
    Digit *d = number->last;
    
    while (d->next != NULL)
        d = d->next;
    
    if (number->sign == 0)
        printf("-");
        
    while (d != NULL) {
        printf("%d",d->value);
        d = d->previous;
    }
    
    printf("\n");
}



int is_digit(int c) {
    return (c >= '0') && (c <= '9');
}

int is_lower_case(int c) {
    return (c >= 'a') && (c <= 'z');
}

Token next_token()
{
}

void goto_next_line()
{
    char c = getchar();
    while (c != '\n')
        if (c == EOF)
            exit(0);
        else
            c = getchar();
}

void calculator(Stack *stack, Number *variables_list[])
{
    Token token = next_token();
    char character;

    while (token.type != END_OF_FILE) {
        switch(token.type)
        {
            case NUMBER:
                push(stack, token.data.num); 
                break;

            case PLUS:
                add(stack);
                break;

            case MINUS:
                sub(stack);
                break;

            case MULTIPLY:
                mul(stack);
                break;
                
            case ASSIGNMENT:
                assignment(token.data.variable, stack->top);
                break;

            case VARIABLE:
                push(stack,
                        variable_value(token.data.variable));
                break;

            case CARRIAGE_RETURN:
                if (stack->nb_elem == 1)
                    print_number(stack->top);
                else
                    printf("\nErreur, entré non-valide\n");
                empty_stack(stack);
                break;

            case ERROR:
                printf("\nErreur, entré non-valide\n");
                empty_stack(stack);
                goto_next_line();
                break;
        }

        token = next_token();
    }

    empty_stack(stack);
    free(stack);
    for (int i = 0; i < 26; i++)
        delete_number(variables_list[i]);
}

int main() {

    Stack *stack = malloc(sizeof(stack));
    Number* variables_list[26];
    calculator(stack, variables_list);

    return 0;
}

