/* TP1 IFT2035
 * by Nicolas Lafond 
 * version 2
 * postfix calculator with infinite precision
 * numbers are stocked in linked list where each digit is stocked in an element
 * the first digit in the list is the last digit of the number represented
 * addition, substraction and multiplication are supported but not division
 */

#include "calculator.h"
/* 
 * this variable is set to TRUE when a token is read and the
 * next character read is a carriage return.
 */
int end_of_line;



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
    if (digit == NULL)
        return NULL;
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

/* Beginning of arithmetic operations section
 * functions with an underscore as first character that for
 * granted that both numbers on the stack are positive. They 
 * are only called by the main arithmetic functions.
 */

// NOTE: return -1 on error
Number* _add(Number *num1, Number *num2)
{

    Number *sum = malloc(sizeof(Number));
    sum->sign = POSITIVE;

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

    return sum;
}

Number* _sub(Number *num1, Number *num2)
{
    //TODO
    return NULL;
}

Number* _mul(Number *num1, Number *num2)
{
    // TODO
    return NULL;
}

int add(Stack *stack)
{
    if (stack->nb_elem < 2)
        return -1;

    Number *num1 = pop(stack);
    Number *num2 = pop(stack);
    Number *sum;

    if (num1->sign == POSITIVE && num2->sign == POSITIVE)
        sum = _add(num1, num2);
    else if (num1->sign == POSITIVE && num2->sign == NEGATIVE)
        sum = _sub(num1, num2);
    else if (num1->sign == NEGATIVE && num2->sign == POSITIVE)
        sum = _sub(num2, num1);
    else if (num1->sign == NEGATIVE && num2->sign == NEGATIVE) {
        sum = _add(num1, num2);
        sum->sign = NEGATIVE;
    }

    push(stack, sum);
    return 0;
}


int sub(Stack *stack)
{
    if (stack->nb_elem < 2)
        return -1;

    Number *num1 = pop(stack);
    Number *num2 = pop(stack);
    Number *diff;

    if (num1->sign == POSITIVE && num2->sign == POSITIVE)
        diff = _sub(num1, num2);
    else if (num1->sign == POSITIVE && num2->sign == NEGATIVE)
        diff = _add(num1, num2);
    else if (num1->sign == NEGATIVE && num2->sign == POSITIVE) {
        diff = _add(num1, num2);
        diff->sign = NEGATIVE;
    }
    else if (num1->sign == NEGATIVE && num2->sign == NEGATIVE)
        diff = _sub(num2, num1);
    

    push(stack, diff);
    return 0;
}


int mul(Stack *stack)
{
    if (stack->nb_elem < 2)
        return -1;

    Number *num1 = pop(stack);
    Number *num2 = pop(stack);
    Number *prod = _mul(num1, num2);

    if (num1->sign == POSITIVE && num2->sign == NEGATIVE)
        prod->sign = NEGATIVE;
    else if (num1->sign == NEGATIVE && num2->sign == POSITIVE)
        prod->sign = NEGATIVE;

    push(stack, prod);
    return 0;
}

void assignment(char variable, Number *num) 
{
}

Number* variable_value(char variable)
{
    // TODO
    return NULL;
}

// Stack functions END

Number* create_number()
{
    Number *num = malloc(sizeof(Number));
    num->sign = POSITIVE;
    num->last = NULL;
    num->next = NULL;
    num->nb_ref = 0;
    return num;
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

void print_number(Number *number) 
{
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



int is_digit(char c) 
{
    return (c >= '0') && (c <= '9');
}

int to_digit(char c) 
{
    if (!is_digit(c))
        return -1;
    else {
        return c - '0';
    }
}

int is_lower_case(char c)
{
    return (c >= 'a') && (c <= 'z');
}

// create a number by reading from stdin
Number* _read_number(char first_digit)
{
    Number *num = malloc(sizeof(Number));
    num->sign = POSITIVE;
    num->nb_ref = 0;
    num->last = create_digit(first_digit, NULL, NULL);
    Digit *current_digit = num->last;
    Digit *new_digit;

    char c;
    int value;
    while (TRUE) {
       c = getchar();
       value = to_digit(c);
       if (value == -1)
           if (c == ' ' || c == '\t')
               return num;
           else if (c == '\n') {
               end_of_line = TRUE;
               return num;
           }
           else { // ERROR
               delete_number(num);
               return NULL;
           }
       else {
          new_digit = create_digit(c, NULL, current_digit);
          if (new_digit == NULL) {
              delete_number(num);
              return NULL;
          }
          current_digit->next = new_digit;
          current_digit = new_digit;
       }

    }
}

Token next_token()
{
    Token token;
    char c;

    // skip blank characters
    while (c = getchar()) {
        if (c == ' ' || c == '\t'); 
        else
            break;
    }

    if (is_digit(c)) {
        Number *num = _read_number(c);
        if (num == NULL)
            token.type = ERROR;
        else
            token.type = NUMBER;
    }
    else if (is_lower_case(c)) {
        char variable = c;
        c = getchar();
        if (c == ' ' || c == '\t') {
            token.type = VARIABLE;
            token.data.variable = variable;
        }
        else if (c == '\n') {
            end_of_line = TRUE;
            token.type = VARIABLE;
            token.data.variable = variable;
        }
        else
            token.type = ERROR;
    }
    else switch(c)
    {
        case '+':
            c = getchar();
            if (c == ' ' || c == '\t') 
                token.type = PLUS;
            else if (c == '\n') {
                end_of_line = TRUE;
                token.type = PLUS;
            }
            else
                token.type = ERROR;
            break;
        case '-':
            c = getchar();

            // minus operation
            if (c == ' ' || c == '\t')
                token.type = MINUS;
            else if (c == '\n') {
                end_of_line = TRUE;
                token.type = MINUS;
            }

            // negative number
            else if (is_digit(c)) {
                Number *num = _read_number(c);
                if (num == NULL)
                    token.type = ERROR;
                else {
                    token.type = NUMBER;
                    num->sign = NEGATIVE;
                }


        case '*':
            c = getchar();
            if (c == ' ' || c == '\t') 
                token.type = MULTIPLY;
            else if (c == '\n') {
                end_of_line = TRUE;
                token.type = MULTIPLY;
            }
            else
                token.type = ERROR;
            break;
        case '=':
            c = getchar();
            if (is_lower_case(c)) {
                char variable = c;
                c = getchar();
                if (c == ' ' || c == '\t') {
                    token.type = ASSIGNMENT;
                    token.data.variable = variable;
                }
                else if (c == '\n') {
                    end_of_line = TRUE;
                    token.type = ASSIGNMENT;
                    token.data.variable = variable;
                }
                else
                    token.type = ERROR;
            }
            else
                token.type = ERROR;
            break;
        case EOF:
            token.type = END_OF_FILE;
            break;
        case '\n':
            token.type = CARRIAGE_RETURN;
            break;
        default:
            token.type = ERROR;
            break;
    }

    return token;
    }
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
    printf(">");
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

        printf(">");
        token = next_token();
    }

    empty_stack(stack);
    free(stack);
    for (int i = 0; i < 26; i++)
        delete_number(variables_list[i]);
}

