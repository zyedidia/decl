#!/bin/sh
#
# Usage: build-lfi-compiler.sh PREFIX ARCH (aarch64 or x86_64)

set -ex

PREFIX=$1

export MARCH=$2
export ARCH=$2-lfi

./build-llvm.sh $PREFIX

cp $ARCH-linux-musl.cfg $PREFIX/bin

# Download a prebuilt sysroot from https://github.com/zyedidia/lfi-llvm-toolchain/releases/tag/prebuilt
