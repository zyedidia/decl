#!/bin/sh
#
# Usage: build-libcxx.sh PREFIX

set -ex

PREFIX=$1

if [ "$MARCH" = "aarch64" ]
then
    LINUX_ARCH=arm64
else
    LINUX_ARCH=$MARCH
fi

make -C linux headers_install ARCH=$LINUX_ARCH INSTALL_HDR_PATH=$PREFIX/sysroot/usr

rm -rf build-libcxx-$ARCH
mkdir -p build-libcxx-$ARCH
cd build-libcxx-$ARCH
cmake -G Ninja ../llvm-project/runtimes \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER=$PREFIX/bin/clang \
    -DCMAKE_CXX_COMPILER=$PREFIX/bin/clang \
    -DLIBCXX_HAS_MUSL_LIBC=YES \
    -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi;libunwind" \
    -DCMAKE_INSTALL_PREFIX=$PREFIX/sysroot/usr \
    -DLIBUNWIND_ENABLE_SHARED=ON \
    -DLIBCXXABI_ENABLE_SHARED=ON \
    -DLIBCXX_ENABLE_SHARED=ON \
    -DLIBCXXABI_LINK_TESTS_WITH_SHARED_LIBCXX=OFF \
    -DLIBCXX_ENABLE_ABI_LINKER_SCRIPT=OFF \
    -DLIBCXX_LINK_TESTS_WITH_SHARED_LIBCXXABI=OFF \
    -DLIBCXX_LINK_TESTS_WITH_SHARED_LIBCXX=OFF \
    -DCMAKE_C_COMPILER_WORKS=ON \
    -DCMAKE_CXX_COMPILER_WORKS=ON
ninja unwind cxxabi cxx
ninja install
