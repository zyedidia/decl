struct MacroInst {
    int size;
    int ninstr;
};

static bool assert_reg(FdInstr* instr, uint8_t op_index, FdReg reg, uint8_t op_size) {
    return (FD_OP_TYPE(instr, op_index) == FD_OT_REG &&
        FD_OP_REG(instr, op_index) == reg &&
        FD_OP_SIZE(instr, op_index) == op_size);
}


// movs and stos instructions, needed for poc
static struct MacroInst macroinst_stos(Verifier* v, uint8_t* buf, size_t size) {
    // stos becomes:
    //
    // movl %edi, %edi
    // leaq (%r14, %rdi), %rdi
    // rep stosq
    // movl %edi, %edi

    FdInstr i_mov, i_lea, i_stos, i_mov2;
    size_t offset = 0;

    if (fd_decode(&buf[offset], size - offset, 64, 0, &i_mov) < 0) {
        return (struct MacroInst){-1, 0};
    }
    offset += i_mov.size;
    if (FD_TYPE(&i_mov) != FDI_MOV ||
        !assert_reg(&i_mov, 0, FD_REG_DI, 4) ||
        !assert_reg(&i_mov, 1, FD_REG_DI, 4)) {
        return (struct MacroInst){-1, 0};
    }

    if (fd_decode(&buf[offset], size - offset, 64, 0, &i_lea) < 0) {
        return (struct MacroInst){-1, 0};
    }
    offset += i_lea.size;
    if (FD_TYPE(&i_lea) != FDI_LEA ||
        !assert_reg(&i_lea, 0, FD_REG_DI, 8) ||
        FD_OP_TYPE(&i_lea, 1) != FD_OT_MEM ||
        FD_OP_BASE(&i_lea, 1) != FD_REG_R14 ||
        FD_OP_INDEX(&i_lea, 1) != FD_REG_DI ||
        FD_OP_DISP(&i_lea, 1) != 0 ||
        FD_OP_SCALE(&i_lea, 1) != 0){
        return (struct MacroInst){-1, 0};
    }

    if (fd_decode(&buf[offset], size - offset, 64, 0, &i_stos) < 0) {
        return (struct MacroInst){-1, 0};
    }
    offset += i_stos.size;
    if (FD_TYPE(&i_stos) != FDI_STOS) {
        return (struct MacroInst){-1, 0};
    }

    if (fd_decode(&buf[offset], size - offset, 64, 0, &i_mov2) < 0) {
        return (struct MacroInst){-1, 0};
    }
    offset += i_mov2.size;
    if (FD_TYPE(&i_mov2) != FDI_MOV ||
        !assert_reg(&i_mov2, 0, FD_REG_DI, 4) ||
        !assert_reg(&i_mov2, 1, FD_REG_DI, 4)) {
        return (struct MacroInst){-1, 0};
    }

    return (struct MacroInst){offset, 4};
}

static struct MacroInst macroinst_movs(Verifier*v, uint8_t* buf, size_t size) {
    // movs becomes:
    //
    // movl %edi, %edi
    // leaq (%r14, %rdi), %rdi
    // movl %esi, %esi
    // leaq (%r14, %rsi), %rsi
    // movs
    // movl %edi, %edi
    // movl %esi, %esi

    FdInstr i_mov, i_lea, i_mov2, i_lea2, i_movs, i_movpost1, i_movpost2;
    size_t offset = 0;

    if (fd_decode(&buf[offset], size - offset, 64, 0, &i_mov) < 0) {
        return (struct MacroInst){-1, 0};
    }
    offset += i_mov.size;
    if (FD_TYPE(&i_mov) != FDI_MOV ||
        !assert_reg(&i_mov, 0, FD_REG_DI, 4) ||
        !assert_reg(&i_mov, 1, FD_REG_DI, 4)) {
        return (struct MacroInst){-1, 0};
    }

    if (fd_decode(&buf[offset], size - offset, 64, 0, &i_lea) < 0) {
        return (struct MacroInst){-1, 0};
    }
    offset += i_lea.size;
    if (FD_TYPE(&i_lea) != FDI_LEA ||
        !assert_reg(&i_lea, 0, FD_REG_DI, 8) ||
        FD_OP_TYPE(&i_lea, 1) != FD_OT_MEM ||
        FD_OP_BASE(&i_lea, 1) != FD_REG_R14 ||
        FD_OP_INDEX(&i_lea, 1) != FD_REG_DI ||
        FD_OP_DISP(&i_lea, 1) != 0 ||
        FD_OP_SCALE(&i_lea, 1) != 0){
        return (struct MacroInst){-1, 0};
    }

    if (fd_decode(&buf[offset], size - offset, 64, 0, &i_mov2) < 0) {
        return (struct MacroInst){-1, 0};
    }
    offset += i_mov2.size;
    if (FD_TYPE(&i_mov2) != FDI_MOV ||
        !assert_reg(&i_mov2, 0, FD_REG_SI, 4) ||
        !assert_reg(&i_mov2, 1, FD_REG_SI, 4)) {
        return (struct MacroInst){-1, 0};
    }

    if (fd_decode(&buf[offset], size - offset, 64, 0, &i_lea2) < 0) {
        return (struct MacroInst){-1, 0};
    }
    offset += i_lea2.size;
    if (FD_TYPE(&i_lea2) != FDI_LEA ||
        !assert_reg(&i_lea2, 0, FD_REG_SI, 8) ||
        FD_OP_TYPE(&i_lea2, 1) != FD_OT_MEM ||
        FD_OP_BASE(&i_lea2, 1) != FD_REG_R14 ||
        FD_OP_INDEX(&i_lea2, 1) != FD_REG_SI ||
        FD_OP_DISP(&i_lea2, 1) != 0 ||
        FD_OP_SCALE(&i_lea2, 1) != 0){
        return (struct MacroInst){-1, 0};
    }

    if (fd_decode(&buf[offset], size - offset, 64, 0, &i_movs) < 0) {
        return (struct MacroInst){-1, 0};
    }
    offset += i_movs.size;
    if (FD_TYPE(&i_movs) != FDI_MOVS) {
        return (struct MacroInst){-1, 0};
    }

    if (fd_decode(&buf[offset], size - offset, 64, 0, &i_movpost1) < 0) {
        return (struct MacroInst){-1, 0};
    }
    offset += i_movpost1.size;
    if (FD_TYPE(&i_movpost1) != FDI_MOV ||
        !assert_reg(&i_movpost1, 0, FD_REG_DI, 4) ||
        !assert_reg(&i_movpost1, 1, FD_REG_DI, 4)) {
        return (struct MacroInst){-1, 0};
    }

    if (fd_decode(&buf[offset], size - offset, 64, 0, &i_movpost2) < 0) {
        return (struct MacroInst){-1, 0};
    }
    offset += i_movpost2.size;
    if (FD_TYPE(&i_movpost2) != FDI_MOV ||
        !assert_reg(&i_movpost2, 0, FD_REG_SI, 4) ||
        !assert_reg(&i_movpost2, 1, FD_REG_SI, 4)) {
        return (struct MacroInst){-1, 0};
    }

    return (struct MacroInst){offset, 7};
}

static struct MacroInst macroinst_jmp(Verifier* v, uint8_t* buf, size_t size) {
    // andl $0xffffffe0, %eX
    // orq %r14, %rX
    // jmpq *%rX

    FdInstr i_and, i_or, i_jmp;
    if (fd_decode(&buf[0], size, 64, 0, &i_and) < 0)
        return (struct MacroInst){-1, 0};
    if (fd_decode(&buf[i_and.size], size - i_and.size, 64, 0, &i_or) < 0)
        return (struct MacroInst){-1, 0};
    if (fd_decode(&buf[i_and.size + i_or.size], size - i_and.size - i_or.size, 64, 0, &i_jmp) < 0)
        return (struct MacroInst){-1, 0};

    if (FD_TYPE(&i_and) != FDI_AND ||
            FD_OP_TYPE(&i_and, 0) != FD_OT_REG ||
            FD_OP_SIZE(&i_and, 0) != 4 ||
            FD_OP_TYPE(&i_and, 1) != FD_OT_IMM ||
            FD_OP_IMM(&i_and, 1) != 0xffffffffffffffe0)
        return (struct MacroInst){-1, 0};

    if (FD_TYPE(&i_or) != FDI_OR ||
            FD_OP_TYPE(&i_or, 0) != FD_OT_REG ||
            FD_OP_TYPE(&i_or, 1) != FD_OT_REG ||
            FD_OP_SIZE(&i_or, 0) != 8 ||
            FD_OP_SIZE(&i_or, 1) != 8 ||
            FD_OP_REG(&i_or, 1) != FD_REG_R14 ||
            FD_OP_REG(&i_or, 0) != FD_OP_REG(&i_and, 0))
        return (struct MacroInst){-1, 0};

    if (FD_TYPE(&i_jmp) != FDI_JMP ||
            FD_OP_TYPE(&i_jmp, 0) != FD_OT_REG ||
            FD_OP_REG(&i_jmp, 0) != FD_OP_REG(&i_and, 0))
        return (struct MacroInst){-1, 0};

    return (struct MacroInst){i_and.size + i_or.size + i_jmp.size, 3};
}

static bool okdisp(int64_t disp) {
    return (disp % 8 == 0) && (disp < 256);
}

static struct MacroInst macroinst_rtcall(Verifier* v, uint8_t* buf, size_t size) {
    // leaq 1f(%rip), %r11
    // jmpq *N(%r13)
    // 1:
    FdInstr i_lea, i_jmp;
    if (fd_decode(&buf[0], size, 64, 0, &i_lea) < 0)
        return (struct MacroInst){-1, 0};
    if (fd_decode(&buf[i_lea.size], size - i_lea.size, 64, 0, &i_jmp) < 0)
        return (struct MacroInst){-1, 0};

    if (FD_TYPE(&i_lea) != FDI_LEA ||
            FD_OP_TYPE(&i_lea, 0) != FD_OT_REG ||
            FD_OP_REG(&i_lea, 0) != FD_REG_R11 ||
            FD_OP_TYPE(&i_lea, 1) != FD_OT_MEM ||
            FD_OP_BASE(&i_lea, 1) != FD_REG_IP)
        return (struct MacroInst){-1, 0};

    if (FD_TYPE(&i_jmp) != FDI_JMP ||
            FD_OP_TYPE(&i_jmp, 0) != FD_OT_MEM ||
            FD_OP_BASE(&i_jmp, 0) != FD_REG_R13 ||
            FD_OP_INDEX(&i_jmp, 0) != FD_REG_NONE ||
            FD_OP_SCALE(&i_jmp, 0) != 0 ||
            !okdisp(FD_OP_DISP(&i_jmp, 0)))
        return (struct MacroInst){-1, 0};

    // Return target can either be the next instruction or can be some
    // bundle-aligned location.
    uintptr_t ret = v->addr + i_lea.size + FD_OP_DISP(&i_lea, 1);
    // Assumes bundle size is 32.
    bool ok = FD_OP_DISP(&i_lea, 1) == i_jmp.size || ret % 32 == 0;
    if (!ok)
        return (struct MacroInst){-1, 0};

    return (struct MacroInst){i_lea.size + i_jmp.size, 2};
}

static size_t bundle(Verifier* v) {
    switch (v->opts->bundle) {
    // case LFI_BUNDLE16:
    //     return 16;
    case LFI_BUNDLE32:
        return 32;
    default:
        assert(!"unreachable");
    }
}

static struct MacroInst macroinst_call(Verifier* v, uint8_t* buf, size_t size) {
    size_t bundlesize = bundle(v);

    // not allowed in position-oblivious-code
    if (v->opts->poc)
        return (struct MacroInst){-1, 0};

    // andl $0xffffffe0, %eX
    // orq %r14, %rX
    // nop*
    // callq *%rX

    FdInstr i_and, i_or, i_jmp;
    if (fd_decode(&buf[0], size, 64, 0, &i_and) < 0)
        return (struct MacroInst){-1, 0};
    if (FD_TYPE(&i_and) != FDI_AND)
        return (struct MacroInst){-1, 0};

    if (fd_decode(&buf[i_and.size], size - i_and.size, 64, 0, &i_or) < 0)
        return (struct MacroInst){-1, 0};
    size_t count = i_and.size + i_or.size;
    size_t icount = 2;
    while (count < bundlesize) {
        if (fd_decode(&buf[count], size - count, 64, 0, &i_jmp) < 0)
            return (struct MacroInst){-1, 0};
        icount++;
        count += i_jmp.size;
        if (FD_TYPE(&i_jmp) != FDI_NOP)
            break;
    }

    if ((v->addr + count) % bundlesize != 0)
        return (struct MacroInst){-1, 0};

    if (FD_TYPE(&i_and) != FDI_AND ||
            FD_OP_TYPE(&i_and, 0) != FD_OT_REG ||
            FD_OP_SIZE(&i_and, 0) != 4 ||
            FD_OP_TYPE(&i_and, 1) != FD_OT_IMM ||
            FD_OP_IMM(&i_and, 1) != 0xffffffffffffffe0)
        return (struct MacroInst){-1, 0};

    if (FD_TYPE(&i_or) != FDI_OR ||
            FD_OP_TYPE(&i_or, 0) != FD_OT_REG ||
            FD_OP_TYPE(&i_or, 1) != FD_OT_REG ||
            FD_OP_SIZE(&i_or, 0) != 8 ||
            FD_OP_SIZE(&i_or, 1) != 8 ||
            FD_OP_REG(&i_or, 1) != FD_REG_R14 ||
            FD_OP_REG(&i_or, 0) != FD_OP_REG(&i_and, 0))
        return (struct MacroInst){-1, 0};

    if (FD_TYPE(&i_jmp) != FDI_CALL ||
            FD_OP_TYPE(&i_jmp, 0) != FD_OT_REG ||
            FD_OP_REG(&i_jmp, 0) != FD_OP_REG(&i_and, 0))
        return (struct MacroInst){-1, 0};

    return (struct MacroInst){count, icount};
}

static struct MacroInst macroinst_modsp(Verifier* v, uint8_t* buf, size_t size) {
    // movl/addl/subl/andl/leal ..., %esp
    // orq %r14, %rsp

    FdInstr i_mov, i_or;
    if (fd_decode(&buf[0], size, 64, 0, &i_mov) < 0)
        return (struct MacroInst){-1, 0};
    if (fd_decode(&buf[i_mov.size], size - i_mov.size, 64, 0, &i_or) < 0)
        return (struct MacroInst){-1, 0};

    // allow addl, subl, lea, or movl
    if (FD_TYPE(&i_mov) != FDI_MOV &&
            FD_TYPE(&i_mov) != FDI_ADD &&
            FD_TYPE(&i_mov) != FDI_AND &&
            FD_TYPE(&i_mov) != FDI_SUB &&
            FD_TYPE(&i_mov) != FDI_LEA)
        return (struct MacroInst){-1, 0};

    if (FD_OP_TYPE(&i_mov, 0) != FD_OT_REG ||
            FD_OP_SIZE(&i_mov, 0) != 4 ||
            FD_OP_REG(&i_mov, 0) != FD_REG_SP)
        return (struct MacroInst){-1, 0};

    // Allow 'orq %r14, %rsp' or 'lea (%rsp, %r14, 1), %rsp'
    bool fail_or = (FD_TYPE(&i_or) != FDI_OR ||
            FD_OP_TYPE(&i_or, 0) != FD_OT_REG ||
            FD_OP_TYPE(&i_or, 1) != FD_OT_REG ||
            FD_OP_SIZE(&i_or, 0) != 8 ||
            FD_OP_SIZE(&i_or, 1) != 8 ||
            FD_OP_REG(&i_or, 0) != FD_REG_SP ||
            FD_OP_REG(&i_or, 1) != FD_REG_R14);
    bool fail_lea = (FD_TYPE(&i_or) != FDI_LEA ||
            FD_OP_TYPE(&i_or, 1) != FD_OT_MEM ||
            FD_OP_BASE(&i_or, 1) != FD_REG_SP ||
            FD_OP_INDEX(&i_or, 1) != FD_REG_R14 ||
            FD_OP_DISP(&i_or, 1) != 0 ||
            FD_OP_SCALE(&i_or, 1) != 0 ||
            FD_OP_TYPE(&i_or, 0) != FD_OT_REG ||
            FD_OP_SIZE(&i_or, 0) != 8 ||
            FD_OP_REG(&i_or, 0) != FD_REG_SP);
    if (fail_lea && fail_or)
        return (struct MacroInst){-1, 0};

    return (struct MacroInst){i_mov.size + i_or.size, 2};
}

static struct MacroInst macroinst_shxd(Verifier* v, uint8_t* buf, size_t size) {
    // andb $0x.. %cl
    // shxd %cl, %rX, %rY
    FdInstr i_andb, i_shxd;
    if (fd_decode(&buf[0], size, 64, 0, &i_andb) < 0)
        return (struct MacroInst){-1, 0};

    if ((FD_TYPE(&i_andb) == FDI_SHRD || FD_TYPE(&i_andb) == FDI_SHLD))
        if (FD_OP_SIZE(&i_andb, 0) == 8)
            return (struct MacroInst){i_andb.size, 1};

    if (fd_decode(&buf[i_andb.size], size - i_andb.size, 64, 0, &i_shxd) < 0)
        return (struct MacroInst){-1, 0};

    if ((FD_TYPE(&i_shxd) != FDI_SHRD && FD_TYPE(&i_shxd) != FDI_SHLD))
        return (struct MacroInst){-1, 0};

    uint8_t opsize = FD_OP_SIZE(&i_shxd, 0);
    uint8_t imm;
    switch (opsize) {
    case 4:
        imm = 0x1f;
        break;
    case 2:
        imm = 0xf;
        break;
    case 1:
        imm = 0x7;
        break;
    default:
        // should be unreachable.
        return (struct MacroInst){-1, 0};
    }

    if (FD_TYPE(&i_andb) != FDI_AND ||
            FD_OP_TYPE(&i_andb, 0) != FD_OT_REG ||
            FD_OP_SIZE(&i_andb, 0) != 1 ||
            FD_OP_REG(&i_andb, 0) != FD_REG_CX ||
            FD_OP_TYPE(&i_andb, 1) != FD_OT_IMM ||
            FD_OP_IMM(&i_andb, 1) != imm)
        return (struct MacroInst){-1, 0};

    return (struct MacroInst){i_andb.size + i_shxd.size, 2};
}

typedef struct MacroInst (*MacroFn)(Verifier*, uint8_t*, size_t);

static MacroFn mfns[] = {
    macroinst_jmp,
    macroinst_call,
    macroinst_rtcall,
    macroinst_modsp,
    macroinst_stos,
    macroinst_movs,
    macroinst_shxd,
};

static struct MacroInst macroinst(Verifier* v, uint8_t* buf, size_t size) {
    for (size_t i = 0; i < sizeof(mfns) / sizeof(mfns[0]); i++) {
        struct MacroInst mi = mfns[i](v, buf, size);
        if (mi.size > 0)
            return mi;
    }
    return (struct MacroInst){-1, 0};
}
