#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "hardware/gpio.h"
#include "stack.h"
#include "opcodes.h"
#include <stdlib.h>

bool stack_empty(stack *S)
{
    assert(S != NULL);
    return S->top == NULL;
}

stack *stack_new()
{
    stack *temp_stack = malloc(sizeof(stack));
    temp_stack->top = NULL;
    assert(temp_stack != NULL);
    return temp_stack;
}

void push(stack *S, void *elem)
{
    assert(S != NULL);
    node *temp_node = malloc(sizeof(node));
    temp_node->data = elem;
    temp_node->next = S->top;
    S->top = temp_node;
}

void *pop(stack *S)
{
    assert(S != NULL);
    assert(!stack_empty(S));
    node *temp_node = S->top;
    S->top = S->top->next;
    void *data = temp_node->data;
    free(temp_node);
    return data;
}

void stack_free(stack *S, elem_free_fn *e)
{
    assert(S != NULL);
    assert(e != NULL);
    while (!stack_empty(S))
    {
        void *temp = pop(S);
        (*e)(temp);
    }
    free(S);
}

void print_buf(const uint8_t *buf, size_t len)
{
    for (size_t i = 0; i < len; ++i)
    {
        printf("%02x", buf[i]);
        if (i % 16 == 15)
            printf("\n");
        else
            printf(" ");
    }
}

uint8_t hex2int(char a, char b)
{
    uint8_t a_val = -1;
    uint8_t b_val = -1;
    if ((uint8_t)a <= 57 && (uint8_t)a >= 48)
    {
        a_val = (uint8_t)a - 48;
    }
    else if ((uint8_t)a <= 70 && (uint8_t)a >= 65)
    {
        a_val = (uint8_t)a - 55;
    }
    else if ((uint8_t)a <= 102 && (uint8_t)a >= 97)
    {
        a_val = (uint8_t)a - 87;
    }
    if ((uint8_t)b <= 57 && (uint8_t)b >= 48)
    {
        b_val = (uint8_t)b - 48;
    }
    else if ((uint8_t)b <= 70 && (uint8_t)b >= 65)
    {
        b_val = (uint8_t)b - 55;
    }
    else if ((uint8_t)b <= 102 && (uint8_t)b >= 97)
    {
        b_val = (uint8_t)b - 87;
    }
    return a_val * 16 + b_val;
}

int32_t execute(uint32_t program_frame, uint8_t numvars)
{
    stack_t S = stack_new();                                  // Stack
    uint32_t pc = 0;                                          // program counter
    uint8_t *P = (uint8_t *)(XIP_BASE + 256 * program_frame); // Bytecode
    print_buf(P, 256);
    void **V = calloc(numvars, sizeof(void *)); // local variables
    for (int i = 0; i <= 28; i++)
        gpio_init(i);
    while (pc < 256)
    {
        switch (P[pc])
        {
        case NOOP: // S -> S
        {
            printf("NOOP\n");
            pc++;
            break;
        }
        case ADD: // S,x,y -> S,x+y
        {
            printf("ADD\n");
            pc++;
            if (stack_empty(S))
            {
                printf("ERR: Stack Empty\n");
                return -1;
            }
            int32_t *a = (int32_t *)pop(S);
            if (stack_empty(S))
            {
                printf("ERR: Stack Empty\n");
                return -1;
            }
            int32_t *b = (int32_t *)pop(S);
            int32_t c = *a + *b;
            int32_t *res = malloc(sizeof(int32_t));
            *res = c;
            free(a);
            free(b);
            push(S, (void *)res);
            break;
        }
        case SUB: // S,x,y -> S,x-y
        {
            printf("SUB\n");
            pc++;
            if (stack_empty(S))
            {
                printf("ERR: Stack Empty\n");
                return -1;
            }
            int32_t *a = (int32_t *)pop(S);
            if (stack_empty(S))
            {
                printf("ERR: Stack Empty\n");
                return -1;
            }
            int32_t *b = (int32_t *)pop(S);
            int32_t c = *b - *a;
            int32_t *res = malloc(sizeof(int32_t));
            *res = c;
            free(a);
            free(b);
            push(S, (void *)res);
            break;
        }
        case MUL: // S,x,y -> S,x*y
        {
            printf("MUL\n");
            pc++;
            if (stack_empty(S))
            {
                printf("ERR: Stack Empty\n");
                return -1;
            }
            int32_t *a = (int32_t *)pop(S);
            if (stack_empty(S))
            {
                printf("ERR: Stack Empty\n");
                return -1;
            }
            int32_t *b = (int32_t *)pop(S);
            int32_t c = (*b) * (*a);
            int32_t *res = malloc(sizeof(int32_t));
            *res = c;
            free(a);
            free(b);
            push(S, (void *)res);
            break;
        }
        case DIV: // S,x,y -> S,x/y
        {
            printf("DIV\n");
            pc++;
            if (stack_empty(S))
            {
                printf("ERR: Stack Empty\n");
                return -1;
            }
            int32_t *a = (int32_t *)pop(S);
            if (stack_empty(S))
            {
                printf("ERR: Stack Empty\n");
                return -1;
            }
            int32_t *b = (int32_t *)pop(S);
            if (*a == 0)
            {
                printf("ERR: Divide by 0");
                return -1;
            }
            int32_t c = (*b) / (*a);
            int32_t *res = malloc(sizeof(int32_t));
            *res = c;
            free(a);
            free(b);
            push(S, (void *)res);
            break;
        }
        case MOD: // S,x,y -> S,x%y
        {
            printf("MOD\n");
            pc++;
            if (stack_empty(S))
            {
                printf("ERR: Stack Empty\n");
                return -1;
            }
            int32_t *a = (int32_t *)pop(S);
            if (stack_empty(S))
            {
                printf("ERR: Stack Empty\n");
                return -1;
            }
            int32_t *b = (int32_t *)pop(S);
            if (*a == 0)
            {
                printf("ERR: Modulo by 0");
                return -1;
            }
            int32_t c = (*b) % (*a);
            int32_t *res = malloc(sizeof(int32_t));
            *res = c;
            free(a);
            free(b);
            push(S, (void *)res);
            break;
        }
        case AND: // S,x,y -> S,x&y
        {
            printf("AND\n");
            pc++;
            if (stack_empty(S))
            {
                printf("ERR: Stack Empty\n");
                return -1;
            }
            int32_t *a = (int32_t *)pop(S);
            if (stack_empty(S))
            {
                printf("ERR: Stack Empty\n");
                return -1;
            }
            int32_t *b = (int32_t *)pop(S);
            int32_t c = (*b) & (*a);
            int32_t *res = malloc(sizeof(int32_t));
            *res = c;
            free(a);
            free(b);
            push(S, (void *)res);
            break;
        }
        case OR: // S,x,y -> S,x|y
        {
            printf("OR\n");
            pc++;
            if (stack_empty(S))
            {
                printf("ERR: Stack Empty\n");
                return -1;
            }
            int32_t *a = (int32_t *)pop(S);
            if (stack_empty(S))
            {
                printf("ERR: Stack Empty\n");
                return -1;
            }
            int32_t *b = (int32_t *)pop(S);
            int32_t c = (*b) | (*a);
            int32_t *res = malloc(sizeof(int32_t));
            *res = c;
            free(a);
            free(b);
            push(S, (void *)res);
            break;
        }
        case XOR: // S,x,y -> S,x^y
        {
            printf("XOR\n");
            pc++;
            if (stack_empty(S))
            {
                printf("ERR: Stack Empty\n");
                return -1;
            }
            int32_t *a = (int32_t *)pop(S);
            if (stack_empty(S))
            {
                printf("ERR: Stack Empty\n");
                return -1;
            }
            int32_t *b = (int32_t *)pop(S);
            int32_t c = (*b) ^ (*a);
            int32_t *res = malloc(sizeof(int32_t));
            *res = c;
            free(a);
            free(b);
            push(S, (void *)res);
            break;
        }
        case RSH: // S,x,y -> S,x >> y
        {
            printf("RSH\n");
            pc++;
            if (stack_empty(S))
            {
                printf("ERR: Stack Empty\n");
                return -1;
            }
            int32_t *a = (int32_t *)pop(S);
            if (stack_empty(S))
            {
                printf("ERR: Stack Empty\n");
                return -1;
            }
            int32_t *b = (int32_t *)pop(S);
            int32_t c = (*b) >> (*a);
            int32_t *res = malloc(sizeof(int32_t));
            *res = c;
            free(a);
            free(b);
            push(S, (void *)res);
            break;
        }
        case LSH: // S,x,y -> S,x << y
        {
            printf("LSH\n");
            pc++;
            if (stack_empty(S))
            {
                printf("ERR: Stack Empty\n");
                return -1;
            }
            int32_t *a = (int32_t *)pop(S);
            if (stack_empty(S))
            {
                printf("ERR: Stack Empty\n");
                return -1;
            }
            int32_t *b = (int32_t *)pop(S); // Be wary of shifting by 32 bits and over
            int32_t c = (*b) << (*a);
            int32_t *res = malloc(sizeof(int32_t));
            *res = c;
            free(a);
            free(b);
            push(S, (void *)res);
            break;
        }
        case PUSH: // S -> S,x
        {
            pc++;
            int32_t val = (int32_t)((uint32_t)P[pc]);
            pc++;
            printf("PUSH %d\n", val);
            int32_t *res = malloc(sizeof(int32_t));
            *res = val;
            push(S, (void *)res);
            break;
        }
        case POP: // S,x -> S
        {
            printf("POP\n");
            pc++;
            void *temp = pop(S);
            free(temp);
            break;
        }
        case LOAD: // S -> S,x
        {
            pc++;
            uint8_t index = P[pc];
            pc++;
            if (index >= numvars)
            {
                printf("ERR: Local Variable Index Out Of Bounds\n");
                return -1;
            }
            printf("LOAD from V[%d]\n", index);
            push(S, V[index]);
            break;
        }
        case STORE: // S,x -> S
        {
            pc++;
            uint8_t index = P[pc];
            pc++;
            if (index >= numvars)
            {
                printf("ERR: Local Variable Index Out Of Bounds\n");
                return -1;
            }
            void *a = pop(S);
            printf("STORE %p at V[%d]\n", a, index);
            V[index] = a;
            break;
        }
        case GPIOIN: // S -> S,x
        {
            printf("GPIOIN\n");
            pc++;
            uint8_t pin = P[pc];
            pc++;
            gpio_set_dir(pin, GPIO_IN);
            bool in = gpio_get(pin);
            int32_t c = (in) ? 1 : 0;
            int32_t *res = malloc(sizeof(int32_t));
            *res = c;
            push(S, (void *)res);
            break;
        }
        case GPIOOUT: // S,x -> S
        {
            printf("GPIOOUT\n");
            pc++;
            uint8_t pin = P[pc];
            pc++;
            void *a = pop(S);
            int32_t c = *((int32_t *)a);
            free(a);
            gpio_set_dir(pin, GPIO_OUT);
            gpio_put(pin, c != 0);
            break;
        }
        case PRINT: // S,00,...,b,a -> S
        {
            printf("PRINT: \"");
            pc++;
            uint8_t c = 0xff;
            while (!stack_empty(S) && c != 0x00)
            {
                void *a = pop(S);
                c = *((uint8_t *)(a));
                free(a);
                printf("%c", (char)c);
            }
            printf("\"\n");
            break;
        }
        case RETURN: // S,x -> END
        {
            printf("RETURN\n");
            pc++;
            void *a = pop(S);
            assert(stack_empty(S));
            int32_t val = *((int32_t *)a);
            free(a);
            // DEINIT and FREE everything
            for (int i = 0; i <= 28; i++)
                gpio_deinit(i);
            stack_free(S, &free);
            for (int i = 0; i < numvars; i++)
            {
                free(V[i]);
            }
            free(V);
            // DONE FREEING
            return val;
        }
        case CALL: // S,x -> NEW FRAME
        {
            printf("CALL\n");
            pc++;
            void *a = pop(S);
            uint32_t frame = *((int32_t *)a);
            free(a);
            execute(frame, 10); // HOW TO FIND OUT NUMVARS
            break;
        }
        case SLEEP: // S,x -> S
        {
            printf("SLEEP\n");
            pc++;
            void *a = pop(S);
            uint32_t time = *((int32_t *)a);
            free(a);
            sleep_ms(time * 1000);
            break;
        }
        default:
        {
            printf("ERR: Unknown OpCode: %d\n", P[pc]);
            return -1;
        }
        }
    }
}

void write(uint32_t program_frame)
{
    printf("In write function!\n");
    uint32_t pc = 0;
    printf("Erasing sector ... \n");
    uint32_t status = save_and_disable_interrupts();
    flash_range_erase(256 * program_frame, 256);
    restore_interrupts(status);
    printf("Done Erasing!\n");
    uint8_t data[256];
    bool editing = true;
    while (pc < 256)
    {
        printf("In Loop pc=%d\n", pc);
        if (!editing)
        {
            printf("Not editing!\n");
            data[pc] = 0;
            pc++;
            continue;
        }
        char a = getchar();
        char b = getchar();
        printf("%c%c\n", a, b);
        uint8_t val = hex2int(a, b);
        printf("val=%d\n", (int)val);
        printf("Setting data[%d]=%d\n", pc, val);
        data[pc] = val;
        pc++;
        if (val == 18)
        {
            printf("Switching editing off!\n");
            editing = false;
            continue;
        }
    }
    printf("Programming sector ...\n");
    status = save_and_disable_interrupts();
    flash_range_program(256 * program_frame, data, 256);
    restore_interrupts(status);
    printf("Done Programming!\n");
}

int main()
{
    stdio_init_all();
    getchar();
    while (true)
    {
        printf("Enter command (r,w,e): ");
        char c = getchar();
        printf("\nEnter FLASH segment MSB (hex 2-digits): ");
        char a = getchar();
        char b = getchar();
        printf("%c%c\n", a, b);
        uint8_t val1 = hex2int(a, b);
        printf("Enter FLASH segment LSB (hex 2-digits): ");
        a = getchar();
        b = getchar();
        printf("%c%c\n", a, b);
        uint8_t val2 = hex2int(a, b);
        uint16_t val = (uint16_t)val1 * 256 + val2;
        if (c == 'r')
        {
            uint8_t *P = (uint8_t *)(XIP_BASE + 256 * val);
            print_buf(P, 256);
        }
        else if (c == 'w')
        {
            printf("Begin writing program:\n");
            write(val); // Write program at val*256
        }
        else if (c == 'e')
        {
            printf("Enter number of variables (hex 2-digits): ");
            a = getchar();
            b = getchar();
            printf("%c%c\n", a, b);
            val2 = hex2int(a, b);
            int32_t res = execute(val, val2); // Execute the program at that location
            printf("Return value: %d\n\n", res);
        }
    }
    return 0;
}