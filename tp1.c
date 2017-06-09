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

typedef enum State {
    INIT,
    DIGIT,
    ERROR,
    EQUAL
} State;

typedef enum token_type {
    NUMBER,
    PLUS,
    MINUS,
    MULTIPLY,
    AFFECTATION,
    VARIABLE
} token_type;

typedef struct Token {
    token_type type;
    union {
        Number* num;
        char variable;
    };
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

void empty_stack(Stack *s)
{
    Number *num;
    while (s->nb_elem > 0) {
        num = pop(s);
        if (num->nb_ref == 0)
            free(num);
    }
}

// NOTE: return -1 on error
int add(Stack *stack)
{
}

int sub(Stack *stack)
{
}

int mul(Stack *stack)
{
}

void assignment(char variable, Number *num) 
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

void printNumber(Number *number) {
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

Digit* create_digit(int value, Digit *next, Digit *previous)
{
    Digit *digit = malloc(sizeof(Digit));
    digit->value = value;
    digit->next = next;
    digit->previous = previous;
    return digit;
}


int is_digit(int c) {
    return (c >= '0') && (c <= '9');
}

int is_lower_case(int c) {
    return (c >= 'a') && (c <= 'z');
}

void next_token()
{
}

int main() {

    int c;
    State state;
    Stack stack;
    Number *n, variables[26];
    Digit *d;
    
    state = INIT;
    printf(">");

    while ( (c = getchar()) != EOF) {
        if (c == '\n' && state == ERROR) {
            printf("error incorrect input\n>");
            emptyStack(&stack);
            state = INIT;
            continue;
        }
        else if (c == '\n') {
            //printValue(stack.top);
            emptyStack(&stack);
            state = INIT;
            continue;
        }

        switch (state) {
            case INIT:
                if (c == ' ' || c == '\t')
                    break;
                else if (is_digit(c)) {
                    d = createDigit(c);
                    n = createNumber(d);
                }
                else if (c == '+')
                    add(&stack);
                else if (c == '-')
                    substract(&stack);
                else if (c == '*')
                    multiply(&stack);
                else if (is_lower_case(c))
                    push(&stack, value(c));
                else 
                    state = ERROR;       
                break;
    
            case DIGIT:
                if (c == ' ' || c == '\t') {
                    push(&stack, n);
                    state = INIT;
                }
                else if (is_digit(c)) {
                    d = createDigit(c);
                    addDigit(n, d);
                }
                else 
                    state = ERROR;
                break;
   
            case EQUAL:
                if (is_lower_case(c) && (stack.nb_elem == 0))
                    state = ERROR;
                else if (is_lower_case(c) && (stack.nb_elem > 0))
                    affectation(stack.top, c);
                else 
                    state = ERROR;
                break;    
         
            case ERROR:           
                break;
        }
    }

    return 0;	
 
}

