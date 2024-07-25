#!/bin/sh

# usage: ./build.sh PREFIX ARCH (aarch64_lfi or x86_64)

set -e

PREFIX=$1

export ARCH=$2

if [ ! -d gcc ]; then
    ./download.sh
    ./setup.sh
fi

mkdir -p $PREFIX
./build-binutils.sh $PREFIX
./build-gcc.sh $PREFIX
