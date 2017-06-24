/* 
 * Postfix calculator with infinite precision
 * by Nicolas Lafond 
 * version 2
 * numbers are stocked in linked list where each digit is stocked in an element
 * the first digit in the list is the last digit of the number represented
 * addition, substraction and multiplication are supported but not (yet) division.
 */

#include "calculator.h"

/* 
 * this variable is set to TRUE when a token is read and the
 * next character read is a carriage return.
 */
int end_of_line = FALSE;

// Stack functions BEGIN
void push(Stack *stack, Number *number)
{
    number->next = stack->top;
    stack->top = number;
    stack->nb_elem++;
}

Number* pop(Stack *stack)
{
    if (stack->nb_elem == 0)
        return NULL;
    else {
        Number *number = stack->top;
        stack->top = stack->top->next;
        number->next = NULL;
        stack->nb_elem--;
        return number;
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
    Digit *digit = (Digit*) malloc(sizeof(Digit));
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

int number_of_digits(int number)
{
    if (number == 0)
        return 1;
    else if (number < 0)
        number = -1 * number;

    int nb_digits = 0;
    while (exponent(10, nb_digits) <= number)
        nb_digits++;

    return nb_digits;
}

// A small function to easily create a number from an int
// mostly for testing purposes, we don't assume that the
// number can be arbitrary long like in the calculator
Number* create_number_from_int(int num)
{
    int nb_digits = number_of_digits(num);
    int position = nb_digits;
    Digit *current_digit, *next_digit;

    current_digit = NULL;
    next_digit = NULL;
    Number *number = (Number*) malloc(sizeof(Number));
    if (number == NULL) {
        printf("insufficient memory to create a number");
        return NULL;
    }
    else if (num < 0)
        number->sign = 0;
    else
        number->sign = 1;

    while (position >= 1) {
        current_digit = nth_digit(num, position);
        if (current_digit == NULL) {
            printf("insufficient memory to create a digit");
            number->last = current_digit;
            delete_number(number);
            return NULL;
        }
        current_digit->next = next_digit;
        if (next_digit != NULL)
            next_digit->previous = current_digit;
        next_digit = current_digit;
        position--;
    }

    number->last = current_digit;

    return number;
}

Number* copy_number(Number *number)
{
    if (number == NULL)
        return NULL;
    Number *copy;
    Digit *number_digit, *copy_digit;
    number_digit = number->last;
    copy = malloc(sizeof(Number));
    if (copy == NULL)
        return NULL;
    copy->last = malloc(sizeof(Digit));
    if (copy->last == NULL)
        delete_number(copy);
    copy->sign = number->sign;
    copy_digit = copy->last;
    copy_digit->previous = NULL;
    copy_digit->value = number_digit->value;
    number_digit = number_digit->next;

    while (number_digit != NULL) {
        copy_digit->next = malloc(sizeof(Digit));
        if (copy_digit->next == NULL) {
            delete_number(copy);
            return NULL;
        }
        copy_digit->next->previous = copy_digit;
        copy_digit = copy_digit->next;
        copy_digit->value = number_digit->value;
        number_digit = number_digit->next;
    }

    return copy;
}

// append number2 to number1
// number1 is changed but number2 is not
// return -1 on error and 0 otherwise
int append_numbers(Number *number1, Number *number2)
{
    if (number1 == NULL || number2 == NULL)
        return -1;
    Digit *digit1 = number1->last;
    Digit *digit2 = number2->last;
    if (digit1 == NULL || digit2 == NULL)
        return -1;

    // set digit2 to the most significant digit of number2
    while (digit2->next != NULL)
        digit2 = digit2->next;

    while (digit2 != NULL) {
        digit1->previous = create_digit(digit2->value, digit1, NULL);
        if (digit1->previous == NULL) { // Not enough memory
            digit1 = number1->last->previous;
            Digit *previous_digit;
            while (digit1 != NULL) {
                previous_digit = digit1->previous;
                free(digit1);
                digit1 = previous_digit;
            }
            return -1;
        }
        else {
            digit1 = digit1->previous;
            digit2 = digit2->previous;
        }
    }

    digit1->previous = NULL;
    number1->last = digit1;
    return 0;
}

void empty_stack(Stack *s)
{
    Number *num;
    while (s->nb_elem > 0) {
        num = pop(s);
        if (num->nb_ref == 0)
            delete_number(num);
    }
}

int is_in_stack(Number *number, Stack *stack)
{
    if (number == NULL || stack == NULL)
        return FALSE;
    Number *stack_number = stack->top;
    while (stack_number != NULL) {
        if (stack_number == number)
            return TRUE;
        stack_number = stack_number->next;
    }

    return FALSE;
}

int greater_equal_first_digit(Number *number1, Number *number2)
{
    if (number1 == NULL || number2 == NULL)
        return FALSE;
    Digit *digit1, *digit2;
    digit1 = number1->last;
    digit2 = number2->last;
    if (digit1 == NULL || digit2 == NULL)
        return FALSE;

    while (digit1->next != NULL)
        digit1 = digit1->next;
    while (digit2->next != NULL)
        digit2 = digit2->next;

    if (digit1->value >= digit2->value)
        return TRUE;
    else
        return FALSE;
}

/* 
 * Beginning of arithmetic operations section
 * functions with an underscore as first character that for
 * granted that both numbers on the stack are positive. They 
 * are only called by the main arithmetic functions.
 */

Digit* add_digits(Digit *digit1, Digit *digit2, int *excess)
{
    if (digit1 == NULL && digit2 == NULL) {
        *excess = 0;
        return NULL;
    }

    Digit *sum = malloc(sizeof(Digit));
    if (sum == NULL) { // Not enough memory
        printf("\nNot enough memory\n");
        return NULL;
    }
    else if ((digit1 == NULL) && (digit2 != NULL)) {
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

Number* _add(Number *num1, Number *num2)
{
    if (num1 == NULL)
        return num2;
    else if (num2 == NULL)
        return num1;

    Number *sum = malloc(sizeof(Number));
    if (sum == NULL) { // Not enough memory
        printf("\nNot enough memory\n");
        return NULL;
    }
    sum->sign = POSITIVE;

    Digit *digit1, *digit2, *digit_sum, *current_digit;
    int excess = 0;

    digit1 = num1->last;
    digit2 = num2->last;

    // this code is a bit ugly, should probably refactor
    digit_sum = add_digits(digit1, digit2, &excess);
    if (digit_sum == NULL) { // Not enough memory
        delete_number(sum);
        return NULL;
    }
    sum->last = digit_sum;
    digit1 = digit1->next;
    digit2 = digit2->next;
    current_digit = digit_sum;

    while ((digit1 != NULL) || (digit2 != NULL)) {
        digit_sum = add_digits(digit1, digit2, &excess);
        if (digit_sum == NULL) { // Not enough memory
            delete_number(sum);
            return NULL;
        }
        digit_sum->previous = current_digit;
        current_digit->next = digit_sum;
        if (digit1 != NULL)
            digit1 = digit1->next;
        if (digit2 != NULL)
            digit2 = digit2->next;
        current_digit = digit_sum;
    }

    if (excess == 1) 
        current_digit->next = create_digit(1, NULL, current_digit);
        if (current_digit == NULL) { // Not enough memory
            delete_number(sum);
            return NULL;
        }

    return sum;
}

Digit* sub_digits(Digit *digit1, Digit *digit2, int *carry)
{
    Digit *diff;
    int value1, value2;

    diff = malloc(sizeof(Digit));
    if (diff == NULL)
        return NULL;

    if (digit2 == NULL) 
        value2 = 0;
    else
        value2 = digit2->value;

    if (*carry == 1 && digit1->value == 0) {
        value1 = 19;
    }
    else if (*carry == 1 && digit1->value != 0) {
        value1 = digit1->value - 1;
        *carry = 0;
    }
    else if (*carry == 0) {
        value1 = digit1->value;
    }

    if (value1 < value2) {
        value1 += 10;
        *carry = 1;
    }

    diff->value = value1 - value2;
    diff->next = NULL;
    diff->previous = NULL;

    return diff;
}

Number* _sub(Number *num1, Number *num2)
{
    // NOTE : this function should only receive two positive number 
    // with |num1| > |num2|
    Number *diff = malloc(sizeof(Number));
    if (diff == NULL)
        return NULL;
    diff->sign = POSITIVE;

    Digit *digit1, *digit2, *digit_diff, *current_digit;
    int carry = 0;

    digit1 = num1->last;
    digit2 = num2->last;

    // same as _add
    digit_diff = sub_digits(digit1, digit2, &carry);
    if (digit_diff == NULL) { // Not enough memory
        delete_number(diff);
        return NULL;
    }
    diff->last = digit_diff;
    digit1 = digit1->next;
    digit2 = digit2->next;
    current_digit = digit_diff;

    while (digit1 != NULL) {
        digit_diff = sub_digits(digit1, digit2, &carry);
        if (digit_diff == NULL) { // Not enough memory
            delete_number(diff);
            return NULL;
        }
        digit_diff->previous = current_digit;
        current_digit->next = digit_diff;
        digit1 = digit1->next;
        if (digit2 != NULL)
            digit2 = digit2->next;
        current_digit = digit_diff;
    }


    return diff;
}

Digit* mul_digits(Digit *digit1, Digit *digit2, int *carry)
{
    Digit *product;
    product = malloc(sizeof(Digit));
    if (product == NULL) { // Not enough memory
        return NULL;
    }
    int product_value = digit1->value * digit2->value + *carry;
    product->value = product_value % 10;
    *carry = (product_value - product->value) / 10;

    return product;
}

// multiply the number by a number composed of a single digit
// useful for _mul() function
Number* _multiply_by_digit(Number *number, Digit *digit)
{
    Number *product = malloc(sizeof(Number));
    if (product == NULL) { // Not enough memory
        return NULL;
    }
    product->sign = POSITIVE;
    Digit *product_digit, *current_digit;
    current_digit = number->last;
    int carry = 0;

    product_digit = mul_digits(current_digit, digit, &carry);
    if (product_digit == NULL) { // Not enough memory
        delete_number(product);
        return NULL;
    }
    product->last = product_digit;
    current_digit = current_digit->next;

    while (current_digit != NULL) {
        product_digit->next = mul_digits(current_digit, digit, &carry);
        if (product_digit == NULL) { // Not enough memory
            delete_number(product);
            return NULL;
        }
        product_digit->next->previous = product_digit;
        product_digit = product_digit->next;
        current_digit = current_digit->next;
    }

    if (carry > 0) {
        product_digit->next = create_digit(carry, NULL, product_digit);
        if (product_digit->next == NULL) { // Not enough memory
            delete_number(product);
            return NULL;
        }
    }

    return product;
}

int is_zero(Number *number)
{
    if (number == NULL) {
        return FALSE;
    }
    else if (number->last != NULL) {
        if (number->last->value == 0 && number->last->next == NULL)
            return TRUE;
        else 
            return FALSE;
    }
    else {
        return FALSE;
    }
}

Number* _mul(Number *num1, Number *num2)
{
    if (num1 == NULL || num2 == NULL)
        return NULL;
    Number *product;
    Number *zero, *zeros;
    Digit *current_digit;
    Number *new_product, *old_product;
    int append1, append2;

    // inititialise product to 0
    product = malloc(sizeof(Number));
    if (product == NULL) { // Not enough memory
        return NULL;
    }
    product->sign = POSITIVE;
    product->last = create_digit(0, NULL, NULL);
    if (product->last == NULL) { // Not enough memory
        delete_number(product);
        return NULL;
    }

    if (is_zero(num1) || is_zero(num2))
        return product;

    // zeros is a list of zeros that are appended to a number
    zero = malloc(sizeof(Number));
    if (zero == NULL) { // Not enough memory
        delete_number(product);
        return NULL;
    }
    zero->last = create_digit(0, NULL, NULL);
    if (zero->last == NULL) { // Not enough memory
        free(zero);
        delete_number(product);
        return NULL;
    }
    zeros = malloc(sizeof(Number));
    if (zeros == NULL) { // Not enough memory
        delete_number(product);
        return NULL;
    }
    zeros->last = create_digit(0, NULL, NULL);
    if (zeros->last == NULL) { // Not enough memory
        free(zeros);
        free(zero->last);
        free(zero);
        delete_number(product);
        return NULL;
    }

    current_digit = num2->last;
    new_product = _multiply_by_digit(num1, current_digit);
    if (new_product == NULL) { // Not enough memory
        delete_number(product);
        free(zeros->last);
        free(zeros);
        free(zero->last);
        free(zero);
        return NULL;
    }
    else 
        product = new_product;
    current_digit = current_digit->next;

    while (current_digit != NULL) {
        new_product = _multiply_by_digit(num1, current_digit);
        if (new_product == NULL) { // Not enough memory
            delete_number(product);
            delete_number(zero);
            delete_number(zeros);
            return NULL;
        }
        append1 = append_numbers(new_product, zeros);
        append2 = append_numbers(zeros, zero); // add a zero to zeros
        if (append1 == -1 || append2 == -1) {
            delete_number(product);
            delete_number(zero);
            delete_number(zeros);
            return NULL;
        }
        old_product = product;
        product = _add(product, new_product);
        if (product == NULL) { // Not enough memory
            delete_number(old_product);
            delete_number(zero);
            delete_number(zeros);
            return NULL;
        }
        free(old_product);
        current_digit = current_digit->next;
    }

    return product;
}

// increment the number by one 
void increment(Number *number)
{
    Number *one, *tempo;
    if (number == NULL)
        return;
    if (number->last == NULL)
        return;

    // Check if number is -1
    if (number->sign == NEGATIVE && 
        number->last->value == 1 && 
        number->last->next == NULL) {
        delete_number(number);
        number = create_number_from_int(0);
        return;
    }

    one = create_number_from_int(1);
    if (one == NULL)
        return;

    if (number->sign == POSITIVE) {
        tempo = _add(number, one);
        tempo->sign = POSITIVE;
    }
    else {
        tempo = _sub(number, one);
        tempo->sign = NEGATIVE;
    }

    delete_number(number);
    delete_number(one);
    number = tempo;
}

void _division(Number *divident, Number *divisor, Number *quotient, Number *remainder)
{
    // Here we consider that all numbers are positive
    if (divident == NULL || divisor == NULL)
        return;

    Number *tempo;

    quotient = create_number_from_int(0);
    if (quotient == NULL)
        return;
    quotient->sign = POSITIVE;

    remainder = copy_number(divident);
    if (remainder == NULL) {
        free(quotient);
        return;
    }
    remainder->sign = POSITIVE;

    while (greater_equal(remainder, divisor)) {
        increment(quotient);
        tempo = _sub(remainder, divisor);
        delete_number(remainder);
        remainder = tempo;
    }

}

Number* _div(Number *num1, Number *num2)
{
    if (num1 == NULL || num2 == NULL)
        return NULL;

    Number *quotient, *remainder;
    _division(num1, num2, quotient, remainder);
    free(remainder);
    if (num1->sign != num2->sign)
        quotient->sign = NEGATIVE;

    return quotient;
}

Number* _mod(Number *num1, Number *num2)
{
    Number *quotient, *remainder;
    _division(num1, num2, quotient, remainder);
    free(quotient);
    if (num1->sign != num2->sign)
        remainder->sign = NEGATIVE;

    return remainder;
}

/*
 * This function returns TRUE if |num1| >= |num2|
 * and FALSE otherwise
 */
int greater_equal (Number *num1, Number *num2)
{
    if (num1 == NULL || num2 == NULL)
        return FALSE;

    Digit *digit1, *digit2;

    digit1 = num1->last;
    digit2 = num2->last;

    if (digit1 == NULL || digit2 == NULL)
        return FALSE;

    // first we check that they have the same number of digits
    while (digit1->next != NULL) {
        if (digit2 == NULL)
            return TRUE;
        digit1 = digit1->next;
        digit2 = digit2->next;
    }

    if (digit2 == NULL)
        return TRUE;
    else if (digit2->next != NULL)
        return FALSE;

    // At this point we know that the two numbers are the same length
    while (digit1 != NULL) {
        if (digit1->value < digit2->value)
            return FALSE;
        else if (digit1->value > digit2->value)
            return TRUE;
        else if (digit1->value == digit2->value) {
            digit1 = digit1->previous;
            digit2 = digit2->previous;
        }
    }

    return TRUE;

}

void reverse_sign(Number *number)
{
    if (number->sign == POSITIVE)
        number->sign = NEGATIVE;
    else
        number->sign = POSITIVE;
}

int add(Stack *stack)
{
    if (stack->nb_elem < 2)
        return -1;

    Number *num1 = pop(stack);
    Number *num2 = pop(stack);
    Number *sum;

    if (num1->sign == POSITIVE && num2->sign == POSITIVE) {
        sum = _add(num1, num2);
    }
    else if (num1->sign == POSITIVE && num2->sign == NEGATIVE) {
        sum = _sub(num1, num2);
    }
    else if (num1->sign == NEGATIVE && num2->sign == POSITIVE) {
        sum = _sub(num2, num1);
    }
    else if (num1->sign == NEGATIVE && num2->sign == NEGATIVE) {
        sum = _add(num1, num2);
        if (sum != NULL)
            sum->sign = NEGATIVE;
    }


    if (num1->nb_ref == 0)
        delete_number(num1);
    if (num2->nb_ref == 0)
        delete_number(num2);

    if (sum == NULL)
        return -1;

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
    int greater;

    if      (num1->sign == POSITIVE && num2->sign == POSITIVE) {
        greater = greater_equal(num1, num2);
        if (greater) {
            diff = _sub(num1, num2);
        }
        else {
            diff = _sub(num2, num1);
            if (diff != NULL)
                diff->sign = NEGATIVE;
        }
    }
    else if (num1->sign == NEGATIVE && num2->sign == NEGATIVE) {
        greater = greater_equal(num2, num1);
        if (greater) {
            diff = _sub(num2, num1);
        }
        else {
            diff = _sub(num1, num2);
            if (diff != NULL)
                diff->sign = NEGATIVE;
        }
    }
    else if (num1->sign == POSITIVE && num2->sign == NEGATIVE) {
        diff = _add(num1, num2);
    }
    else if (num1->sign == NEGATIVE && num2->sign == POSITIVE) {
        diff = _add(num1, num2);
        if (diff != NULL)
            diff->sign = NEGATIVE;
    }
    
    if (num1->nb_ref == 0)
        delete_number(num1);
    if (num2->nb_ref == 0)
        delete_number(num2);

    if (diff == NULL)
        return -1;

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
    if (prod == NULL)
        return -1;

    if (num1->sign == POSITIVE && num2->sign == NEGATIVE)
        prod->sign = NEGATIVE;
    else if (num1->sign == NEGATIVE && num2->sign == POSITIVE)
        prod->sign = NEGATIVE;

    if (num1->nb_ref == 0)
        delete_number(num1);
    if (num2->nb_ref == 0)
        delete_number(num2);

    push(stack, prod);
    return 0;
}

int divide(Stack *stack)
{
    if (stack->nb_elem < 2)
        return -1;

    Number *num1 = pop(stack);
    Number *num2 = pop(stack);
    Number *quot = _div(num1, num2);
    if (quot == NULL)
        return -1;

    if (num1->sign == POSITIVE && num2->sign == NEGATIVE)
        quot->sign = NEGATIVE;
    else if (num1->sign == NEGATIVE && num2->sign == POSITIVE)
        quot->sign = NEGATIVE;

    if (num1->nb_ref == 0)
        delete_number(num1);
    if (num2->nb_ref == 0)
        delete_number(num2);

    push(stack, quot);
    return 0;
}

int mod(Stack *stack)
{

    if (stack->nb_elem < 2)
        return -1;

    Number *num1 = pop(stack);
    Number *num2 = pop(stack);
    Number *rest = _mod(num1, num2);
    if (rest == NULL)
        return -1;

    if (num1->sign == POSITIVE && num2->sign == NEGATIVE)
        rest->sign = NEGATIVE;
    else if (num1->sign == NEGATIVE && num2->sign == POSITIVE)
        rest->sign = NEGATIVE;

    if (num1->nb_ref == 0)
        delete_number(num1);
    if (num2->nb_ref == 0)
        delete_number(num2);

    push(stack, rest);
    return 0;
}

void assignment(char variable, Number *num, Number *variables_list[]) 
{
    Number *previous_number = variables_list[variable - 'a'];
    if (previous_number == num) // double assignment the same variable to a number
        return;
    else if (previous_number != NULL && previous_number->nb_ref == 1) {
        delete_number(previous_number);
    } 
    else if (previous_number != NULL && previous_number->nb_ref > 1) {
        previous_number->nb_ref--;
    }
    variables_list[variable - 'a'] = num;
    num->nb_ref += 1;
}

Number* variable_value(char variable, Number *variables_list[])
{
    return variables_list[variable - 'a'];
}

// Stack functions END

Number* create_number()
{
    Number *num = malloc(sizeof(Number));
    if (num == NULL)
        return NULL;
    num->sign = POSITIVE;
    num->last = NULL;
    num->next = NULL;
    num->nb_ref = 0;
    return num;
}

void delete_number(Number *num)
{
    if (num == NULL)
        return;

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
}

void print_token(Token *token)
{
    if (token == NULL)
        return;
    switch(token->type)
    {
        case NUMBER:
            printf("NUMBER: "); 
            print_number(token->data.num);
            break;
        case PLUS:
            printf("PLUS");   
            break;
        case MINUS:
            printf("MINUS");  
            break;
        case MULTIPLY:
            printf("MULTIPLY"); 
            break;
        case ASSIGNMENT:
            printf("ASSIGNMENT (%c)", token->data.variable);
            break;
        case VARIABLE:
            printf("VARIABLE (%c)", token->data.variable);
            break;
        case CARRIAGE_RETURN:
            printf("CARRIAGE_RETURN");
            break;
        case END_OF_FILE:
            printf("END_OF_FILE");
            break;
        case ERROR:
            printf("ERROR");
            break;
    }
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

int is_valid_number(Number *number)
{
    // Check if the number doesn't start with zeros except if the number is zero
    // example : 0 is valid, 0245 is not
    if (number == NULL)
        return FALSE;
    Digit *digit = number->last;
    if (digit == NULL) {
        return FALSE;
    }
    else if (digit->value == 0 && digit->next == NULL) {
        return TRUE;
    }
    else {
        while (digit->next != NULL)
            digit = digit->next;
        if (digit->value == 0)
            return FALSE;
        else
            return TRUE;
    }
}

// create a number by reading from stdin
// this function should only be called by next_token()
// return NULL on error
Number* _read_number(char first_digit)
{
    Number *num = (Number*) malloc(sizeof(Number));
    if (num == NULL)
        return NULL;
    num->sign = POSITIVE;
    num->nb_ref = 0;
    Digit *current_digit = 
        create_digit(to_digit(first_digit), NULL, NULL);
    Digit *new_digit;
    if (current_digit == NULL) {
        free(num);
        return NULL;
    }

    char c;
    int value;
    while (TRUE) {
       c = getchar();
       value = to_digit(c);
       if (value == -1)
           if (c == ' ' || c == '\t') {
               current_digit->previous = NULL;
               num->last = current_digit;
               if (is_valid_number(num)) {
                   return num;
               }
               else {
                   delete_number(num);
                   return NULL;
               }
           }
           else if (c == '\n') {
               current_digit->previous = NULL;
               num->last = current_digit;
               end_of_line = TRUE;
               if (is_valid_number(num)) {
                   return num;
               }
               else {
                   delete_number(num);
                   return NULL;
               }
           }
           else { // ERROR
               delete_number(num);
               return NULL;
           }
       else {
          new_digit = create_digit(value, current_digit, NULL);
          if (new_digit == NULL) {
              delete_number(num);
              return NULL;
          }
          current_digit->previous = new_digit;
          current_digit = new_digit;
       }

    }
}

Token next_token()
{
    Token token;
    char c;

    if (end_of_line) {
        token.type = CARRIAGE_RETURN;
        end_of_line = FALSE;
        return token;
    }

    // skip blank characters
    while (c = getchar()) {
        if (c == ' ' || c == '\t'); 
        else
            break;
    }

    if (is_digit(c)) {
        Number *num = _read_number(c);
        if (num == NULL) {
            token.type = ERROR;
        }
        else {
            token.type = NUMBER;
            token.data.num = num;
        }
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
            if (c == ' ' || c == '\t') {
                token.type = PLUS;
            }
            else if (c == '\n') {
                end_of_line = TRUE;
                token.type = PLUS;
            }
            else {
                token.type = ERROR;
            }
            break;
        case '-':
            c = getchar();

            // minus operation
            if (c == ' ' || c == '\t') {
                token.type = MINUS;
            }
            else if (c == '\n') {
                end_of_line = TRUE;
                token.type = MINUS;
            }

            // negative number
            else if (is_digit(c)) {
                Number *num = _read_number(c);
                if (num == NULL) {
                    token.type = ERROR;
                }
                else {
                    token.type = NUMBER;
                    token.data.num = num;
                    num->sign = NEGATIVE;
                }
            }
            break;

        case '*':
            c = getchar();
            if (c == ' ' || c == '\t') {
                token.type = MULTIPLY;
            }
            else if (c == '\n') {
                end_of_line = TRUE;
                token.type = MULTIPLY;
            }
            else {
                token.type = ERROR;
            }
            break;
        case '/':
            c = getchar();
            if (c == ' ' || c == '\t') {
                token.type = DIVIDE;
            }
            else if (c == '\n') {
                end_of_line = TRUE;
                token.type = DIVIDE;
            }
            else {
                token.type = ERROR;
            }
            break;
        case '%':
            c = getchar();
            if (c == ' ' || c == '\t') {
                token.type = MODULO;
            }
            else if (c == '\n') {
                end_of_line = TRUE;
                token.type = MODULO;
            }
            else {
                token.type = ERROR;
            }
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
                else {
                    token.type = ERROR;
                }
            }
            else {
                token.type = ERROR;
            }
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

// go to the next line ignoring all tokens
// used by calculator function
void goto_next_line()
{
    if (!end_of_line) {
        char c = getchar();
        while (c != '\n') {
            if (c == EOF)
                exit(0);
            else
                c = getchar();
        }
    }
}

void cleanup(Stack *stack, Number *variables_list[])
{
    empty_stack(stack);
    free(stack);

    Number *number;
    for (int i = 0; i < 26; i++) {
        number = variables_list[i];
        if (number != NULL && number->nb_ref > 1) {
            for (int j = i + 1; j < 26; j++) {
                if (variables_list[j] == number)
                    variables_list[j] = NULL;
            }
        }
        delete_number(number);
    }
}

void calculator(Stack *stack, Number *variables_list[])
{
    printf("> ");
    Token token = next_token();
    char character;
    Number *num;

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

            case DIVIDE:
                divide(stack);
                break;

            case MODULO:
                mod(stack);
                break;
                
            case ASSIGNMENT:
                if (stack->top != NULL)
                    assignment(token.data.variable, stack->top, variables_list);
                else
                    printf("Error trying to assign a variable to nothing");
                break;

            case VARIABLE:
                num = variable_value(token.data.variable, variables_list);
                if (num != NULL)
                    push(stack, num);
                else
                    printf("Error trying to access an non-assigned variable");
                break;

            case CARRIAGE_RETURN:
                if (stack->nb_elem == 1)
                    print_number(stack->top);
                else
                    printf("Error wrong number of value entered");
                empty_stack(stack);
                printf("\n> ");
                break;

            case ERROR:
                printf("Error non-valid entry");
                empty_stack(stack);
                goto_next_line();
                printf("\n> ");
                break;
        }

        token = next_token();
    }
    
    cleanup(stack, variables_list);
    printf("\ngoodbye!\n");
}

