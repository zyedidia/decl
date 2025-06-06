#!/bin/bash
set -ex

export MAKEFLAGS="-j32"

export PATH=/opt/decl/bin:$PATH

ALL="gcc_r mcf_r x264_r omnetpp_r xalancbmk_r leela_r deepsjeng_r xz_r parest_r namd_r lbm_r povray_r imagick_r nab_r"
INT="gcc_r mcf_r x264_r omnetpp_r xalancbmk_r leela_r deepsjeng_r xz_r "
WASM_INT="mcf_r x264_r deepsjeng_r xz_r"

runcpu --action=run --size=ref $INT --config=decl-hw-clang --iterations=3
LOADER="lfi-run --poc -- " runcpu --action=run --size=ref $INT --config=decl-lfi-branch-clang --iterations=3
LOADER="lfi-run --poc -- " runcpu --action=run --size=ref $INT --config=decl-lfi-timer-clang --iterations=3
LOADER="lfi-run --poc -- " runcpu --action=run --size=ref $INT --config=decl-lfi-poc-clang --iterations=3
LOADER="lfi-run -- " runcpu --action=run --size=ref $INT --config=lfi-clang --iterations=3
runcpu --action=run --size=ref $INT --config=native-clang --iterations=3

LOADER="wasmtime -W fuel=18446744073709551615 --dir=. --dir=/home" runcpu --config=wasm-lto --size=ref --action=run $WASM_INT --iterations=3
LOADER="wasmtime --dir=. --dir=/home" runcpu --config=wasm-lto --size=ref --action=run $WASM_INT --iterations=3
