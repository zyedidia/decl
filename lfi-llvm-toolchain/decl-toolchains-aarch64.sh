#!/bin/sh

./build-native.sh $PWD/native-clang aarch64

./build-lfi.sh $PWD/lfi-clang aarch64

LFIFLAGS="--decl --sandbox=bundle-jumps --allow-tls --allow-syscall" ./build-lfi.sh $PWD/decl-hw-clang aarch64

LFIFLAGS="--decl --poc" ./build-lfi.sh $PWD/decl-lfi-poc-clang aarch64

LFIFLAGS="--decl --poc --meter=branch" ./build-lfi.sh $PWD/decl-lfi-branch-clang aarch64

LFIFLAGS="--decl --poc --meter=timer" ./build-lfi.sh $PWD/decl-lfi-timer-clang aarch64
