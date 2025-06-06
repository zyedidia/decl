#!/bin/sh

./build-native.sh $PWD/aarch64-native-clang aarch64

./build-lfi.sh $PWD/aarch64-lfi-clang aarch64

LFIFLAGS="--sandbox=stores" ./build-lfi.sh $PWD/aarch64-lfi-stores-clang aarch64
