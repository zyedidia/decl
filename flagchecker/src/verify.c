#include "verifier.h"
#include "fadec.h"

#include <stdlib.h>
#include <assert.h>

bool branchinfo(Verifier* v, FdInstr* instr, int64_t* target, bool* indirect, bool* cond) {
    *target = 0;
    *indirect = false;
    *cond = false;

    // TODO: don't count runtime calls as branches

    bool branch = true;
    switch (FD_TYPE(instr)) {
    case FDI_JA:
    case FDI_JBE:
    case FDI_JC:
    case FDI_JCXZ:
    case FDI_JG:
    case FDI_JGE:
    case FDI_JL:
    case FDI_JLE:
    case FDI_JNC:
    case FDI_JNO:
    case FDI_JNP:
    case FDI_JNS:
    case FDI_JNZ:
    case FDI_JO:
    case FDI_JP:
    case FDI_JS:
    case FDI_JZ:
        *cond = true;
    case FDI_JMP:
    case FDI_CALL:
    case FDI_RET:
        if (FD_OP_TYPE(instr, 0) == FD_OT_OFF) {
            int64_t imm = FD_OP_IMM(instr, 0);
            *target = v->addr + FD_SIZE(instr) + imm;
        } else {
            *indirect = true;
        }
        break;
    default:
        branch = false;
        break;
    }
    return branch;
}

bool flag_verify(void* code, size_t size, uintptr_t addr) {
    uint8_t* insns = (uint8_t*) code;

    Verifier v = {
        .addr = addr,
    };

    size_t count = 0;
    size_t ninstr = 0;
    while (count < size) {
        FdInstr instr;
        int ret = fd_decode(&insns[count], size - count, 64, 0, &instr);
        if (ret < 0) {
            verrmin(&v, "%lx: unknown instruction", v.addr);
            return false;
        }
        ninstr++;
        count += ret;
    }
    printf("counted %ld instrs\n", ninstr);

    // Check flags.
    FdInstr* instrs = malloc(sizeof(FdInstr) * ninstr);
    assert(instrs);
    count = 0;
    size_t i = 0;
    while (count < size) {
        int ret = fd_decode(&insns[count], size - count, 64, 0, &instrs[i++]);
        // must have decoded earlier
        assert(ret >= 0);
        count += ret;
    }

    analyzecfg(&v, instrs, ninstr, addr);
    free(instrs);

    return !v.failed;
}
