srcs = [
  'engine.c',
  'err.c',
  'proc.c',
  'buddy.c',
  'stack.c',
]

mmap = subproject('libmmap').get_variable('mmap')

subdir('arch/' + cpuarch)

liblfi = static_library(
  'lfi',
  srcs,
  c_args: c_args_hardened + ['-fvisibility=hidden'],
  install: true,
  dependencies: [mmap.as_link_whole()],
)

install_headers('lfi.h')

liblfi_dep = declare_dependency(link_with: liblfi, include_directories: include_directories('.'))

subdir('test')
