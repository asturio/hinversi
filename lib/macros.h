/* Handy macros for programming */
#define max(a, b) (a > b) ? a : b
#define min(a, b) (a < b) ? a : b
#define strEq(a, b) (!strcmp(a, b)) ? 1 : 0
#define strEqN(a, b, n) (!strncmp(a, b, n)) ? 1 : 0
