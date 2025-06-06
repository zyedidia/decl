static bool okreadpoc(Verifier* v, struct Da64Inst* dinst, struct Da64Op* op) {
    bool sf = false;
    bool sp = false;
    switch (op->type) {
    case DA_OP_REGSP:
        sp = true;
    case DA_OP_REGGP:
        sf = op->reggp.sf;
        break;
    case DA_OP_REGGPEXT:
        sf = op->reggpext.sf;
        break;
    default:
        return true;
    }

    if (dinst->mnem == DA64I_ADD_EXT) {
        // 'add addrreg, base, lo, uxtw' is allowed.
        if (addrreg(v, dinst->ops[0].reg, true) && dinst->ops[0].reggp.sf == 1 && basereg(dinst->ops[1].reg) &&
                dinst->ops[2].reggpext.ext == DA_EXT_UXTW &&
                dinst->ops[2].reggpext.sf == 0 && dinst->ops[2].reggpext.shift == 0)
            return true;
    }

    if (retreg(op->reg) && dinst->mnem == DA64I_LDR_IMM) {
        // 'ldr x30, [rtsysreg]' is allowed.
        if (dinst->ops[1].type == DA_OP_MEMUOFF && dinst->ops[1].uimm16 == 0 &&
                rtsysreg(v, dinst->ops[1].reg))
            return true;
    }

    if (v->opts->bundle != LFI_BUNDLE_NONE && dinst->mnem == DA64I_AND_IMM) {
        // 'bic x24, x18, 0x[f7]' is allowed
        if (cfreg(v, dinst->ops[0].reg) && addrreg(v, dinst->ops[1].reg, false) &&
                dinst->ops[2].type == DA_OP_IMMLARGE) {
            if (v->opts->bundle == LFI_BUNDLE8 && dinst->imm64 == 0xfffffffffffffff8)
                return true;
            else if (v->opts->bundle == LFI_BUNDLE16 && dinst->imm64 == 0xfffffffffffffff0)
                return true;
        }
    }

    if ((fixedreg(v, op->reg) || addrreg(v, op->reg, sp)) && sf)
        return false;
    return true;
}

static void chkadr(Verifier* v, struct Da64Inst* dinst, uint32_t next_insn) {
    if (!v->opts->poc)
        return;
    if (dinst->mnem == DA64I_ADR || dinst->mnem == DA64I_ADRP) {
        assert(dinst->ops[0].type == DA_OP_REGGP);
        struct Da64Inst next;
        da64_decode(next_insn, &next);
        if (next.mnem != DA64I_ORR_SHIFT ||
                next.ops[0].type != DA_OP_REGGP ||
                next.ops[0].reggp.sf != 0 ||
                next.ops[1].type != DA_OP_REGGP ||
                next.ops[1].reggp.sf != 0 ||
                next.ops[2].type != DA_OP_REGGPEXT ||
                next.ops[2].reggpext.sf != 0 ||
                next.ops[2].reggpext.shift != 0)
            verr(v, dinst, "adr/adrp must be followed by instruction that zeroes top 32 bits");
    }
}
