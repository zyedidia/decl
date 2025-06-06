#!/bin/sh
#
# Usage: build-native.sh PREFIX ARCH (aarch64 or x86_64)

set -ex

PREFIX=$1

export MARCH=$2
export ARCH=$2-unknown

./build-llvm.sh $PREFIX
./build-compiler-rt.sh $PREFIX
./build-musl.sh $PREFIX
./build-libcxx.sh $PREFIX
./build-mimalloc.sh $PREFIX
