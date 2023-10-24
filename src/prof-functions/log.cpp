#include <stdio.h>

extern "C" void funcStartLogger(char* funcName) {
    printf("[F_START] '%s'\n", funcName);
}

extern "C" void callLogger(char* calleeName, char* callerName, long int valID) {
    printf("[CALL] '%s' -> '%s' {%ld}\n", calleeName, callerName, valID);
}

extern "C" void funcEndLogger(char* funcName, long int valID) {
    printf("[F_END] '%s' {%ld}\n", funcName, valID);
}

extern "C" void binOptLogger(int val, int arg0, int arg1, char* opName, char* funcName, long int valID) {
    printf("[BIN_OP] In function '%s': %d = %d %s %d {%ld}\n", funcName, val, arg0, opName, arg1, valID);
}
