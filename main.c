#include "calculator.h"

int main() 
{

    Stack *stack = malloc(sizeof(stack));
    Number* variables_list[26];
    calculator(stack, variables_list);

    return 0;
}
