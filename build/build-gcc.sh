#!/bin/sh

# usage: ./build-gcc.sh PREFIX

set -ex

PREFIX=$1

mkdir -p build-gcc
cd build-gcc
../gcc/configure --target=$ARCH-linux-musl \
    --disable-docs \
    --disable-bootstrap \
    --disable-libssp \
    --disable-multilib \
    --disable-shared \
    --enable-languages=c,c++,fortran \
    --enable-lto \
    --prefix=$PREFIX \
    --with-pkgversion="LFI"

make all-gcc -j$(nproc --all)
make install-strip-gcc

export LFIFLAGS="--decl -a amd64 --single-thread --sandbox=bundle-jumps"

mkdir -p lib/gcc
../specgen.sh > lib/gcc/specs
../specgen.sh > gcc/specs

mkdir -p $PREFIX/$ARCH-linux-musl/lib
../specgen.sh > $PREFIX/$ARCH-linux-musl/lib/specs

# install musl headers

cd ../musl-1.2.4
make clean
CC=$PREFIX/bin/$ARCH-linux-musl-gcc ./configure --prefix=$PREFIX --syslibdir=$PREFIX/$ARCH-linux-musl/lib --libdir=$PREFIX/lib/gcc/$ARCH-linux-musl/13.2.0 --includedir=$PREFIX/$ARCH-linux-musl/include
# first install musl headers
make install-headers

cd ../build-gcc

# now we can build libgcc (requires libc headers)
make all-target-libgcc -j$(nproc --all)
make install-target-libgcc

cd ..

cp musl-custom/getopt.c musl-1.2.4/src/misc/getopt.c
cp musl-custom/x86_64/memset.s musl-1.2.4/src/string/x86_64/memset.s
cp musl-custom/x86_64/memcpy.s musl-1.2.4/src/string/x86_64/memcpy.s

cd musl-1.2.4

make clean

CC=$PREFIX/bin/$ARCH-linux-musl-gcc ./configure --prefix=$PREFIX --syslibdir=$PREFIX/$ARCH-linux-musl/lib --libdir=$PREFIX/lib/gcc/$ARCH-linux-musl/13.2.0 --includedir=$PREFIX/$ARCH-linux-musl/include --disable-shared

# now we can build libc (requires libgcc)
make -j$(nproc --all)
make install

# copy musl installation to build-gcc/gcc

cd ../build-gcc

cp -r $PREFIX/lib/gcc/$ARCH-linux-musl/13.2.0/* gcc

# now build libstdc++ (requires libc and libgcc)
make all-target-libstdc++-v3 -j$(nproc --all)
make install-target-libstdc++-v3

# add linux/limits.h

mkdir -p $PREFIX/$ARCH-linux-musl/include/linux
cp /usr/include/linux/limits.h $PREFIX/$ARCH-linux-musl/include/linux

# now build libgfortran

make all-target-libgfortran -j$(nproc --all)
make install-target-libgfortran
