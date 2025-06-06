#!/bin/sh

./build-native.sh $PWD/native-clang x86_64

./build-lfi.sh $PWD/lfi-clang x86_64

LFIFLAGS="--decl --sandbox=bundle-jumps --allow-tls --allow-syscall" ./build-lfi.sh $PWD/decl-hw-clang x86_64

LFIFLAGS="--decl --poc" ./build-lfi.sh $PWD/decl-lfi-poc-clang x86_64

LFIFLAGS="--decl --poc --meter=branch" ./build-lfi.sh $PWD/decl-lfi-branch-clang x86_64

LFIFLAGS="--decl --poc --meter=timer" ./build-lfi.sh $PWD/decl-lfi-timer-clang x86_64
