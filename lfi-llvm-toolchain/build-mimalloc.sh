#!/bin/sh
#
# Usage: build-mimalloc.sh PREFIX

set -ex

PREFIX=$1

cd mimalloc
rm -rf build
mkdir build
cd build
cmake .. -G Ninja -DMI_OPT_ARCH=OFF -DMI_INSTALL_TOPLEVEL=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=$1/bin/clang -DCMAKE_CXX_COMPILER=$1/bin/clang++ -DCMAKE_INSTALL_PREFIX=$PREFIX/sysroot/usr
ninja
ninja install
