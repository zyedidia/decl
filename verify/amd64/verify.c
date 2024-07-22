#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <Zydis/Zydis.h>
#include <Zycore/Format.h>

#include "flags.h"
#include "insn.h"

enum {
    BUNDLE_SIZE    = 16,
};

static unsigned mnemonics[ZYDIS_MNEMONIC_MAX_VALUE];

typedef struct context {
    ZydisDecoder decoder;
    ZydisDecoderContext zcontext;
    ZydisDecodedInstruction instr;
    ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];

    uint8_t* insns;
    size_t n;
    size_t addr;
    size_t count;

    size_t s_addr;
    size_t s_count;
    ZydisDecodedInstruction s_instr;
    ZydisDecoderContext s_zcontext;
} Context;

static bool
ctxnext(Context* ctx)
{
    if (ctx->instr.length != 0) {
        ctx->addr += ctx->instr.length;
        ctx->count += ctx->instr.length;
    }
    ZyanStatus status = ZydisDecoderDecodeInstruction(&ctx->decoder, &ctx->zcontext, &ctx->insns[ctx->count], ctx->n - ctx->count, &ctx->instr);
    printf("decode %lx, %ld\n", ctx->addr, ctx->instr.length);
    if (ZYAN_FAILED(status)) {
        return false;
    }
    mnemonics[ctx->instr.mnemonic]++;

    return true;
}

static void
ctxsave(Context* ctx)
{
    ctx->s_addr = ctx->addr;
    ctx->s_count = ctx->count;
    ctx->s_instr = ctx->instr;
    ctx->s_zcontext = ctx->zcontext;
}

static void
ctxrestore(Context* ctx)
{
    ctx->addr = ctx->s_addr;
    ctx->count = ctx->s_count;
    ctx->instr = ctx->s_instr;
    ctx->zcontext = ctx->s_zcontext;
}

static bool
ctxoperands(Context* ctx)
{
    ZyanStatus status = ZydisDecoderDecodeOperands(&ctx->decoder, &ctx->zcontext, &ctx->instr, ctx->operands, ctx->instr.operand_count);
    if (ZYAN_FAILED(status)) {
        return false;
    }
    return true;
}

static void
ctxdisplay(FILE* stream, Context* ctx)
{
    ctxoperands(ctx);

    ZydisFormatter formatter;
    ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_ATT);

    char buffer[256];

    ZydisFormatterFormatInstruction(&formatter, &ctx->instr, ctx->operands,
            ctx->instr.operand_count_visible, &buffer[0], sizeof(buffer), ctx->addr,
            ZYAN_NULL);
    fprintf(stream, "%s\n", &buffer[0]);
}

static bool
loreg(ZydisRegister reg)
{
    switch (reg) {
    case ZYDIS_REGISTER_EAX:
    case ZYDIS_REGISTER_ECX:
    case ZYDIS_REGISTER_EDX:
    case ZYDIS_REGISTER_EBX:
    case ZYDIS_REGISTER_ESP:
    case ZYDIS_REGISTER_EBP:
    case ZYDIS_REGISTER_ESI:
    case ZYDIS_REGISTER_EDI:
    case ZYDIS_REGISTER_R8D:
    case ZYDIS_REGISTER_R9D:
    case ZYDIS_REGISTER_R10D:
    case ZYDIS_REGISTER_R11D:
    case ZYDIS_REGISTER_R12D:
    case ZYDIS_REGISTER_R13D:
    case ZYDIS_REGISTER_R14D:
    case ZYDIS_REGISTER_R15D:
        return true;
    }
    return false;
}

static ZydisRegister
hi(ZydisRegister reg)
{
    switch (reg) {
    case ZYDIS_REGISTER_EAX:
        return ZYDIS_REGISTER_RAX;
    case ZYDIS_REGISTER_ECX:
        return ZYDIS_REGISTER_RCX;
    case ZYDIS_REGISTER_EDX:
        return ZYDIS_REGISTER_RDX;
    case ZYDIS_REGISTER_EBX:
        return ZYDIS_REGISTER_RBX;
    case ZYDIS_REGISTER_ESP:
        return ZYDIS_REGISTER_RSP;
    case ZYDIS_REGISTER_EBP:
        return ZYDIS_REGISTER_RBP;
    case ZYDIS_REGISTER_ESI:
        return ZYDIS_REGISTER_RSI;
    case ZYDIS_REGISTER_EDI:
        return ZYDIS_REGISTER_RDI;
    case ZYDIS_REGISTER_R8D:
        return ZYDIS_REGISTER_R8;
    case ZYDIS_REGISTER_R9D:
        return ZYDIS_REGISTER_R9;
    case ZYDIS_REGISTER_R10D:
        return ZYDIS_REGISTER_R10;
    case ZYDIS_REGISTER_R11D:
        return ZYDIS_REGISTER_R11;
    case ZYDIS_REGISTER_R12D:
        return ZYDIS_REGISTER_R12;
    case ZYDIS_REGISTER_R13D:
        return ZYDIS_REGISTER_R13;
    case ZYDIS_REGISTER_R14D:
        return ZYDIS_REGISTER_R14;
    case ZYDIS_REGISTER_R15D:
        return ZYDIS_REGISTER_R15;
    }
    assert(0);
}

static bool
check_instr(Context* ctx)
{
    return mnemonic_valid(ctx->instr.mnemonic);
}

static bool
check_jump(Context* ctx)
{
    switch (ctx->instr.mnemonic) {
    case ZYDIS_MNEMONIC_CALL:
    case ZYDIS_MNEMONIC_JB:
    case ZYDIS_MNEMONIC_JBE:
    case ZYDIS_MNEMONIC_JL:
    case ZYDIS_MNEMONIC_JLE:
    case ZYDIS_MNEMONIC_JMP:
    case ZYDIS_MNEMONIC_JNB:
    case ZYDIS_MNEMONIC_JNBE:
    case ZYDIS_MNEMONIC_JNL:
    case ZYDIS_MNEMONIC_JNLE:
    case ZYDIS_MNEMONIC_JNO:
    case ZYDIS_MNEMONIC_JNP:
    case ZYDIS_MNEMONIC_JNS:
    case ZYDIS_MNEMONIC_JNZ:
    case ZYDIS_MNEMONIC_JO:
    case ZYDIS_MNEMONIC_JP:
    case ZYDIS_MNEMONIC_JS:
    case ZYDIS_MNEMONIC_JZ:
        if (!ctxoperands(ctx)) {
            return false;
        }
        switch (ctx->operands[0].type) {
        case ZYDIS_OPERAND_TYPE_IMMEDIATE:
            size_t target = ctx->addr + ctx->operands[0].imm.value.u + ctx->instr.length;
            if (target % BUNDLE_SIZE != 0) {
                return false;
            }
            return true;
        }
        return false;
    case ZYDIS_MNEMONIC_RET:
        return false;
    }

    return true;
}

static bool
ret_sequence(Context* ctx)
{
    ctxsave(ctx);
    if (ctx->instr.mnemonic != ZYDIS_MNEMONIC_AND)
        goto fail;
    ctxoperands(ctx);
    if (ctx->operands[1].type != ZYDIS_OPERAND_TYPE_IMMEDIATE || ctx->operands[1].imm.value.u != 0xfffffffffffffff0)
        goto fail;
    if (ctx->operands[0].type != ZYDIS_OPERAND_TYPE_MEMORY)
        goto fail;
    ZydisDecodedOperandMem* addr = &ctx->operands[0].mem;
    if (!(addr->type == ZYDIS_MEMOP_TYPE_MEM && addr->base == ZYDIS_REGISTER_RSP && addr->disp.value == 0 && addr->scale == 0 && addr->index == ZYDIS_REGISTER_NONE))
        goto fail;
    if (!ctxnext(ctx))
        goto fail;

    if (ctx->instr.mnemonic != ZYDIS_MNEMONIC_CMP)
        goto fail;

    if (!ctxnext(ctx))
        goto fail;

    if (ctx->instr.mnemonic != ZYDIS_MNEMONIC_RET)
        goto fail;

    return true;
fail:
    ctxrestore(ctx);
    return false;
}

static bool
jump_sequence(Context* ctx)
{
    ctxsave(ctx);
    if (ctx->instr.mnemonic != ZYDIS_MNEMONIC_AND)
        goto fail;
    ctxoperands(ctx);
    if (ctx->operands[1].type != ZYDIS_OPERAND_TYPE_IMMEDIATE || ctx->operands[1].imm.value.u != 0xfffffffffffffff0)
        goto fail;
    if (ctx->operands[0].type != ZYDIS_OPERAND_TYPE_REGISTER)
        goto fail;
    ZydisRegister reg = ctx->operands[0].reg.value;

    if (!ctxnext(ctx))
        goto fail;

    if (ctx->instr.mnemonic != ZYDIS_MNEMONIC_CMP)
        goto fail;

    if (!ctxnext(ctx))
        goto fail;

    if (ctx->instr.mnemonic == ZYDIS_MNEMONIC_NOP)
        if (!ctxnext(ctx))
            goto fail;

    if (ctx->instr.mnemonic != ZYDIS_MNEMONIC_CALL && ctx->instr.mnemonic != ZYDIS_MNEMONIC_JMP)
        goto fail;
    ctxoperands(ctx);
    if (ctx->operands[0].type != ZYDIS_OPERAND_TYPE_REGISTER || ctx->operands[0].reg.value != reg)
        goto fail;

    return true;
fail:
    ctxrestore(ctx);
    return false;
}

static bool
verify_bundle(Context* ctx)
{
    size_t start = ctx->count;
    printf("%lx: start: %ld\n", ctx->addr, start);
    while (ctx->count - start < BUNDLE_SIZE) {
        if (!ctxnext(ctx)) {
            fprintf(stderr, "%lx: decode error\n", ctx->addr);
            return false;
        }
        if (ctx->count - start >= BUNDLE_SIZE) {
            break;
        }
        if (!check_instr(ctx)) {
            fprintf(stderr, "%lx: check_instr error\n", ctx->addr);
            ctxdisplay(stderr, ctx);
            return false;
        }
        if (jump_sequence(ctx)) {
            continue;
        }

        if (ret_sequence(ctx)) {
            continue;
        }

        if (!check_jump(ctx)) {
            fprintf(stderr, "%lx: invalid jump\n", ctx->addr);
            ctxdisplay(stderr, ctx);
            return false;
        }
    }

    printf("%ld %ld\n", ctx->count, start);
    size_t count = ctx->count - start;

    if (count != BUNDLE_SIZE) {
        fprintf(stderr, "%lx: incorrectly sized bundle %ld\n", ctx->addr, count);
        ctxdisplay(stderr, ctx);
    }

    return count == BUNDLE_SIZE;
}

bool
verify(uint8_t* insns, size_t n, size_t baddr)
{
    Context ctx = (Context) {
        .insns = insns,
        .n = n,
        .addr = baddr,
    };
    ZydisDecoderInit(&ctx.decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);

    bool failed = false;

    size_t count = 0;
    while (count < n) {
        ctx.count = count;
        ctx.addr = baddr + count;
        ctx.instr.length = 0;
        if (!verify_bundle(&ctx))
            failed = true;
        count += BUNDLE_SIZE;
    }

    if (failed) {
        return false;
    } else if (ctx.count != ctx.n) {
        fprintf(stderr, "stopped decoding at %lx\n", ctx.addr);
        return false;
    }

    return true;
}
