This is the runtime for integration into Groundhog, with additional support for
"micro-sandboxes" that can start up without system calls on the hot path by
using page aliasing.

Build with

```
meson setup build
cd build
ninja
```
