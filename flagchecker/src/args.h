#pragma once

#include <string.h>
#include <stdbool.h>

enum {
    INPUTMAX = 128,
};

struct Args {
    int n;
    char* inputs[INPUTMAX];
    size_t ninputs;
};

extern struct Args args;
