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
    IF,      // 15 Need to test
    GOTO,    // 16 Need to test
    NOT,     // 17 Need to test
    LT,      // 18 Need to test
    GT,      // 19 Need to test
    EQ,      // 1a Need to test
    LTE,     // 1b Need to test
    GTE,     // 1c Need to test
};