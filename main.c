#include "calculator.h"

int main() 
{

    Stack *stack = malloc(sizeof(stack));
    Number* variables_list[26];
    for (int i = 0; i < 26; i++)
        variables_list[i] = NULL;
    calculator(stack, variables_list);

    return 0;
}
