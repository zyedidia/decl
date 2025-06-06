#!/bin/sh

cargo build --release --target=x86_64-native-linux-musl.json -Z build-std=core,alloc,std

cargo build --release --target=x86_64-decl_branch-linux-musl.json -Z build-std=core,alloc,std

cargo build --release --target=x86_64-decl_timer-linux-musl.json -Z build-std=core,alloc,std

RUSTFLAGS="-C target-feature=+simd128" cargo build --release --target=wasm32-wasip1
mv target/wasm32-wasip1 target/wasm32-wasip1-simd

cargo build --release --target=wasm32-wasip1

# this config of wasm is slower for some reason
# cargo build --release --target=wasm32-wasip1 -Z build-std=core,alloc,std,panic_abort
