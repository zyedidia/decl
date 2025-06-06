#!/bin/sh

set -ex

sp1split ../elf/bn254/riscv32im-succinct-zkvm-elf proof-bn254-groth16.bin
sp1split ../elf/bn254/riscv32im-succinct-zkvm-elf proof-bn254-plonk.bin

sp1split ../elf/chess/riscv32im-succinct-zkvm-elf proof-chess-groth16.bin
sp1split ../elf/chess/riscv32im-succinct-zkvm-elf proof-chess-plonk.bin

sp1split ../elf/fibonacci/riscv32im-succinct-zkvm-elf proof-fibonacci-groth16.bin
sp1split ../elf/fibonacci/riscv32im-succinct-zkvm-elf proof-fibonacci-plonk.bin
