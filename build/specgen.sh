#!/bin/sh

if [ "$ARCH" = "x86_64" ]
then
    LFIARCH=amd64
elif [ "$ARCH" = "aarch64_lfi" ] || [ "$ARCH" = "aarch64" ]
then
    LFIARCH=arm64
elif [ "$ARCH" = "riscv64" ]
then
    LFIARCH=riscv64
    RVSPEC="%{!march:-march=rv64g_zba}"
fi

cat << EOM
@assembler:
%{!M:%{!MM:%{!E:%{!S:lfi-leg $LFIFLAGS -a $LFIARCH %i | as %(asm_debug) %(asm_options) %A }}}}

@assembler-with-cpp:
%(trad_capable_cpp) -lang-asm %(cpp_options) -fno-directives-only %{E|M|MM:%(cpp_debug_options)} %{!M:%{!MM:%{!E:%{!S:-o %|.s |
   lfi-leg $LFIFLAGS -a $LFIARCH %m.s | as %(asm_debug) %(asm_options) %A }}}}

*invoke_as:
%{!fwpa*:   %{fcompare-debug=*|fdump-final-insns=*:%:compare-debug-dump-opt()}   %{!S:-o %|.s |
 lfi-leg $LFIFLAGS -a $LFIARCH %m.s | as %(asm_options) %A }  }

*cc1:
+ $(lfi-leg -a $LFIARCH --flags=gcc $LFIFLAGS) -fPIC -fno-plt

*link:
+ -z separate-code

*self_spec:
%{!shared:%{!static:%{!static-pie:-static-pie}}} -ftls-model=local-exec $RVSPEC

*post_link:
lfi-postlink %{o*:%*;:a.out} -a $LFIARCH $(lfi-leg -a $LFIARCH --flags=postlink $LFIFLAGS)
EOM
