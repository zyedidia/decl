#include <Zydis/Zydis.h>

#include "flags.h"

// See Intel manual Table A-2

// Flags read by instructions. This table must be complete.
enum flags flagread[ZYDIS_MNEMONIC_MAX_VALUE] = {
    [ZYDIS_MNEMONIC_AAA] = F_AF,
    [ZYDIS_MNEMONIC_AAS] = F_AF,
    [ZYDIS_MNEMONIC_ADC] = F_CF,

    [ZYDIS_MNEMONIC_CMOVB]   = CC_B,
    [ZYDIS_MNEMONIC_CMOVBE]  = CC_BE,
    [ZYDIS_MNEMONIC_CMOVL]   = CC_L,
    [ZYDIS_MNEMONIC_CMOVLE]  = CC_LE,
    [ZYDIS_MNEMONIC_CMOVNB]  = CC_NB,
    [ZYDIS_MNEMONIC_CMOVNBE] = CC_NBE,
    [ZYDIS_MNEMONIC_CMOVNL]  = CC_NL,
    [ZYDIS_MNEMONIC_CMOVNLE] = CC_NLE,
    [ZYDIS_MNEMONIC_CMOVNO]  = CC_NO,
    [ZYDIS_MNEMONIC_CMOVNP]  = CC_NP,
    [ZYDIS_MNEMONIC_CMOVNS]  = CC_NS,
    [ZYDIS_MNEMONIC_CMOVNZ]  = CC_NZ,
    [ZYDIS_MNEMONIC_CMOVO]   = CC_O,
    [ZYDIS_MNEMONIC_CMOVP]   = CC_P,
    [ZYDIS_MNEMONIC_CMOVS]   = CC_S,
    [ZYDIS_MNEMONIC_CMOVZ]   = CC_Z,

    [ZYDIS_MNEMONIC_CMPSB] = F_DF,
    [ZYDIS_MNEMONIC_CMPSD] = F_DF,
    [ZYDIS_MNEMONIC_CMPSQ] = F_DF,
    [ZYDIS_MNEMONIC_CMPSS] = F_DF,
    [ZYDIS_MNEMONIC_CMPSW] = F_DF,
    [ZYDIS_MNEMONIC_DAA]   = F_AF | F_CF,
    [ZYDIS_MNEMONIC_DAS]   = F_AF | F_CF,

    [ZYDIS_MNEMONIC_FCMOVB]   = CC_B,
    [ZYDIS_MNEMONIC_FCMOVBE]  = CC_BE,
    [ZYDIS_MNEMONIC_FCMOVE]   = CC_E,
    [ZYDIS_MNEMONIC_FCMOVNB]  = CC_NB,
    [ZYDIS_MNEMONIC_FCMOVNBE] = CC_NBE,
    [ZYDIS_MNEMONIC_FCMOVNE]  = CC_NE,
    [ZYDIS_MNEMONIC_FCMOVNU]  = CC_NU,
    [ZYDIS_MNEMONIC_FCMOVU]   = CC_U,

    [ZYDIS_MNEMONIC_INSB] = F_DF,
    [ZYDIS_MNEMONIC_INSD] = F_DF,
    [ZYDIS_MNEMONIC_INTO] = F_OF,
    [ZYDIS_MNEMONIC_IRET] = F_NT,

    [ZYDIS_MNEMONIC_JB]   = CC_B,
    [ZYDIS_MNEMONIC_JBE]  = CC_BE,
    [ZYDIS_MNEMONIC_JL]   = CC_L,
    [ZYDIS_MNEMONIC_JLE]  = CC_LE,
    [ZYDIS_MNEMONIC_JNB]  = CC_NB,
    [ZYDIS_MNEMONIC_JNBE] = CC_NBE,
    [ZYDIS_MNEMONIC_JNL]  = CC_NL,
    [ZYDIS_MNEMONIC_JNLE] = CC_NLE,
    [ZYDIS_MNEMONIC_JNO]  = CC_NO,
    [ZYDIS_MNEMONIC_JNP]  = CC_NP,
    [ZYDIS_MNEMONIC_JNS]  = CC_NS,
    [ZYDIS_MNEMONIC_JNZ]  = CC_NZ,
    [ZYDIS_MNEMONIC_JO]   = CC_O,
    [ZYDIS_MNEMONIC_JP]   = CC_P,
    [ZYDIS_MNEMONIC_JS]   = CC_S,
    [ZYDIS_MNEMONIC_JZ]   = CC_Z,

    [ZYDIS_MNEMONIC_LODSB]    = F_DF,
    [ZYDIS_MNEMONIC_LODSD]    = F_DF,
    [ZYDIS_MNEMONIC_LODSQ]    = F_DF,
    [ZYDIS_MNEMONIC_LODSW]    = F_DF,
    [ZYDIS_MNEMONIC_LOOPE]    = F_ZF,
    [ZYDIS_MNEMONIC_LOOPNE]   = F_ZF,
    [ZYDIS_MNEMONIC_MOVSB]    = F_DF,
    [ZYDIS_MNEMONIC_MOVSD]    = F_DF,
    [ZYDIS_MNEMONIC_MOVSHDUP] = F_DF,
    [ZYDIS_MNEMONIC_MOVSLDUP] = F_DF,
    [ZYDIS_MNEMONIC_MOVSQ]    = F_DF,
    [ZYDIS_MNEMONIC_MOVSS]    = F_DF,
    [ZYDIS_MNEMONIC_MOVSW]    = F_DF,
    [ZYDIS_MNEMONIC_MOVSX]    = F_DF,
    [ZYDIS_MNEMONIC_MOVSXD]   = F_DF,
    [ZYDIS_MNEMONIC_OUTSB]    = F_DF,
    [ZYDIS_MNEMONIC_OUTSD]    = F_DF,
    [ZYDIS_MNEMONIC_OUTSW]    = F_DF,

    [ZYDIS_MNEMONIC_RCL] = F_CF,
    [ZYDIS_MNEMONIC_RCR] = F_CF,

    [ZYDIS_MNEMONIC_SBB]   = F_CF,

    [ZYDIS_MNEMONIC_SCASB] = F_DF,
    [ZYDIS_MNEMONIC_SCASD] = F_DF,
    [ZYDIS_MNEMONIC_SCASQ] = F_DF,
    [ZYDIS_MNEMONIC_SCASW] = F_DF,

    [ZYDIS_MNEMONIC_SETB]   = CC_B,
    [ZYDIS_MNEMONIC_SETBE]  = CC_BE,
    [ZYDIS_MNEMONIC_SETL]   = CC_L,
    [ZYDIS_MNEMONIC_SETLE]  = CC_LE,
    [ZYDIS_MNEMONIC_SETNB]  = CC_NB,
    [ZYDIS_MNEMONIC_SETNBE] = CC_NBE,
    [ZYDIS_MNEMONIC_SETNL]  = CC_NL,
    [ZYDIS_MNEMONIC_SETNLE] = CC_NLE,
    [ZYDIS_MNEMONIC_SETNO]  = CC_NO,
    [ZYDIS_MNEMONIC_SETNP]  = CC_NP,
    [ZYDIS_MNEMONIC_SETNS]  = CC_NS,
    [ZYDIS_MNEMONIC_SETNZ]  = CC_NZ,
    [ZYDIS_MNEMONIC_SETO]   = CC_O,
    [ZYDIS_MNEMONIC_SETP]   = CC_P,
    [ZYDIS_MNEMONIC_SETS]   = CC_S,
    [ZYDIS_MNEMONIC_SETZ]   = CC_Z,

    [ZYDIS_MNEMONIC_STOSB] = F_DF,
    [ZYDIS_MNEMONIC_STOSD] = F_DF,
    [ZYDIS_MNEMONIC_STOSQ] = F_DF,
    [ZYDIS_MNEMONIC_STOSW] = F_DF,
};

// Flags assigned by instruction. This table can be incomplete. For each
// mnemonic, 1 indicates flag defined, 0 indicates flag undefined.
// This table only covers the 6 flags OF-CF. These are the only flags
// that can be placed in an undefined state.
enum flags flagassign[ZYDIS_MNEMONIC_MAX_VALUE] = {
    [ZYDIS_MNEMONIC_ADD]  = F_OF | F_SF | F_ZF | F_AF | F_PF | F_CF,
    [ZYDIS_MNEMONIC_AND]  = F_OF | F_SF | F_ZF |        F_PF | F_CF,
    [ZYDIS_MNEMONIC_CMP]  = F_OF | F_SF | F_ZF | F_AF | F_PF | F_CF,
    [ZYDIS_MNEMONIC_DEC]  = F_OF | F_SF | F_ZF | F_AF | F_PF,
    [ZYDIS_MNEMONIC_IMUL] = F_OF |                             F_CF,
    [ZYDIS_MNEMONIC_INC]  = F_OF | F_SF | F_ZF | F_AF | F_PF,
    [ZYDIS_MNEMONIC_LSL]  =               F_ZF,
    [ZYDIS_MNEMONIC_NEG]  = F_OF | F_SF | F_ZF | F_AF | F_PF | F_CF,
    [ZYDIS_MNEMONIC_OR]   = F_OF | F_SF | F_ZF |        F_PF | F_CF,
    [ZYDIS_MNEMONIC_SUB]  = F_OF | F_SF | F_ZF | F_AF | F_PF | F_CF,
    [ZYDIS_MNEMONIC_TEST] = F_OF | F_SF | F_ZF |        F_PF | F_CF,
    [ZYDIS_MNEMONIC_XOR]  = F_OF | F_SF | F_ZF |        F_PF | F_CF,
};
