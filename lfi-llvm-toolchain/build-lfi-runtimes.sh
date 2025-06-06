#!/bin/sh
#
# Usage: build-lfi-runtimes.sh PREFIX ARCH

set -ex

PREFIX=$1

export MARCH=$2
export ARCH=$2-lfi

./build-compiler-rt.sh $PREFIX
./build-musl.sh $PREFIX
./build-libcxx.sh $PREFIX
