#!/bin/sh
#
# Usage: build-musl.sh PREFIX

set -ex

PREFIX=$1

export CC=$PREFIX/bin/clang
cd musl
make clean
./configure --prefix=$PREFIX/sysroot/usr --syslibdir=$PREFIX/sysroot/lib
make
make install

# Make the linker symlink relative
rm $PREFIX/sysroot/lib/ld-musl-$MARCH.so.1
ln -sf ../usr/lib/libc.so $PREFIX/sysroot/lib/ld-musl-$MARCH.so.1
