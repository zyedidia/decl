This is the Linux emulation runtime used for running Linux-based benchmark programs.

```
meson setup build
cd build
ninja
```

This produces `liblfix/lfi-run`, the tool that is used to run benchmark binaries.
