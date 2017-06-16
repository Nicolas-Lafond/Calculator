#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TRUE 1
#define FALSE 0

// Constants for the sign of numbers
#define POSITIVE 1
#define NEGATIVE 0


// a structure for digits in a number
// the next pointer point to the next more significant digit
// the previous pointer point to the next less significant digit
typedef struct Digit
{
    int value;
    struct Digit *next;
    struct Digit *previous;
} Digit;

// Represent a number in the stack
typedef struct Number
{
    int sign;  
    struct Digit *last; // pointer to the least significant digit
    struct Number *next; // pointer to the element below on the stack
    int nb_ref; // number of variables who refere to that number
} Number;

typedef struct Stack
{
    struct Number *top; 
    int nb_elem;  
} Stack;


typedef enum token_type
{
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

typedef struct Token
{
    token_type type;
    union {
        Number* num;   // if type == NUMBER
        char variable; // if type == VARIABLE or ASSIGNMENT
    } data;
} Token;

// stack functions
void push(Stack *s, Number *a);
Number* pop(Stack *s);

int exponent(int number, int exp);
Digit* create_digit(int value, Digit *next, Digit *previous);
Digit* nth_digit(int number, int position);
Number* create_number_from_int(int num);
void empty_stack(Stack *s);
Digit* add_digits(Digit *digit1, Digit *digit2, int *excess);

// arithmetics functions
Number* _add(Number *num1, Number *num2);
Number* _sub(Number *num1, Number *num2);
Number* _mul(Number *num1, Number *num2);
int add(Stack *stack);
int sub(Stack *stack);
int mul(Stack *stack);

// other numbers functions
void assignment(char variable, Number *num);
Number* variable_value(char variable);
Number* create_number();
void delete_number(Number *num);
void print_number(Number *number);
void print_token(Token *token);
int is_digit(char c);
int to_digit(char c);
int is_lower_case(char c);
Number* _read_number(char first_digit);

Token next_token();
void goto_next_line();
void calculator(Stack *stack, Number *variables_list[]);
