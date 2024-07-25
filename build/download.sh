#!/bin/sh

set -e

# binutils
BINUTILS=binutils-2.41
wget https://ftp.gnu.org/gnu/binutils/$BINUTILS.tar.xz
tar -xf $BINUTILS.tar.xz
mv $BINUTILS binutils
rm $BINUTILS.tar.xz

# gcc
GCC=gcc-13.2.0
wget https://gcc.gnu.org/pub/gcc/releases/$GCC/$GCC.tar.xz
tar -xf $GCC.tar.xz
mv $GCC gcc
rm $GCC.tar.xz

MUSL=musl-1.2.4
wget https://musl.libc.org/releases/$MUSL.tar.gz
tar -xf $MUSL.tar.gz
