enum Opcode
{
    NOOP,    // 00
    ADD,     // 01
    SUB,     // 02
    MUL,     // 03
    DIV,     // 04
    MOD,     // 05
    AND,     // 06
    OR,      // 07
    XOR,     // 08
    RSH,     // 09
    LSH,     // 0a
    PUSH,    // 0b
    POP,     // 0c
    LOAD,    // 0d
    STORE,   // 0e
    GPIOIN,  // 0f Need to test
    GPIOOUT, // 10
    PRINT,   // 11
    RETURN,  // 12
    CALL,    // 13
    SLEEP,   // 14
    IF,      // 15
    GOTO,    // 16
};