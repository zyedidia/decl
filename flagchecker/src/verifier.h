#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>

#include "fadec.h"

// internal
typedef struct {
    bool failed;
    bool abort;
    uintptr_t addr;
} Verifier;

#define ERRMAX 128

static inline void verrmin(Verifier* v, const char* fmt, ...) {
    v->failed = true;

    va_list ap;

    char errbuf[ERRMAX];

    va_start(ap, fmt);
    vsnprintf(errbuf, ERRMAX, fmt, ap);
    va_end(ap);

    fputs(errbuf, stderr);
}

static inline void verr(Verifier* v, FdInstr* inst, const char* msg) {
    char fmtbuf[64];
    fd_format(inst, fmtbuf, sizeof(fmtbuf));
    verrmin(v, "%x: %s: %s", v->addr, fmtbuf, msg);
}

bool branchinfo(Verifier* v, FdInstr* instr, int64_t* target, bool* indirect, bool* cond);

void analyzecfg(Verifier* v, FdInstr* instrs, size_t n, size_t addr);
