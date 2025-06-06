#!/bin/sh

set -e

PROOF=proof/proof-bn254-groth16.bin

hyperfine --runs 4 --warmup 2 \
    "./target/x86_64-native-linux-musl/release/sp1-verify-std -groth16 $PROOF.proof $PROOF.pub $PROOF.vk" \
    "lfi-run --poc -- ./target/x86_64-decl_timer-linux-musl/release/sp1-verify-std -groth16 $PROOF.proof $PROOF.pub $PROOF.vk" \
    "lfi-run --poc -- ./target/x86_64-decl_branch-linux-musl/release/sp1-verify-std -groth16 $PROOF.proof $PROOF.pub $PROOF.vk" \
    "wasmtime -W fuel=18446744073709551615 --dir=. ./target/wasm32-wasip1-simd/release/sp1-verify-std.wasm -groth16 $PROOF.proof $PROOF.pub $PROOF.vk" \
    "wasmtime -W fuel=18446744073709551615 --dir=. ./target/wasm32-wasip1/release/sp1-verify-std.wasm -groth16 $PROOF.proof $PROOF.pub $PROOF.vk" \
    "wasm3 ./target/wasm32-wasip1/release/sp1-verify-std.wasm -groth16 $PROOF.proof $PROOF.pub $PROOF.vk"

PROOF=proof/proof-bn254-plonk.bin

hyperfine --runs 4 --warmup 2 \
    "./target/x86_64-native-linux-musl/release/sp1-verify-std -plonk $PROOF.proof $PROOF.pub $PROOF.vk" \
    "lfi-run --poc -- ./target/x86_64-decl_timer-linux-musl/release/sp1-verify-std -plonk $PROOF.proof $PROOF.pub $PROOF.vk" \
    "lfi-run --poc -- ./target/x86_64-decl_branch-linux-musl/release/sp1-verify-std -plonk $PROOF.proof $PROOF.pub $PROOF.vk" \
    "wasmtime -W fuel=18446744073709551615 --dir=. ./target/wasm32-wasip1-simd/release/sp1-verify-std.wasm -plonk $PROOF.proof $PROOF.pub $PROOF.vk" \
    "wasmtime -W fuel=18446744073709551615 --dir=. ./target/wasm32-wasip1/release/sp1-verify-std.wasm -plonk $PROOF.proof $PROOF.pub $PROOF.vk" \
    "wasm3 ./target/wasm32-wasip1/release/sp1-verify-std.wasm -plonk $PROOF.proof $PROOF.pub $PROOF.vk"
