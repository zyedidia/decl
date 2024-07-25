#!/bin/sh

# usage: build-binutils PREFIX

set -ex

PREFIX=$1

mkdir -p build-binutils
cd build-binutils
../binutils/configure --target=$ARCH-linux-musl \
    --disable-docs \
    --disable-nls \
    --disable-multilib \
    --prefix=$PREFIX \
    --with-pkgversion="LFI"
make -j$(nproc --all)
make install-strip

cd ..
