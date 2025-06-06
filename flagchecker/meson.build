project(
  'flagchecker',
  ['c'],
  default_options: ['warning_level=1', 'buildtype=release']
)

c_args_hardened = [
  '-fno-strict-aliasing',
  '-fno-delete-null-pointer-checks',
  '-fno-strict-overflow',
  '-Werror=implicit',
  '-Werror=incompatible-pointer-types',
  '-Werror=int-conversion',
]
compiler = meson.get_compiler('c')
has_auto_var_init = compiler.has_argument('-ftrivial-auto-var-init=zero')
if has_auto_var_init
  c_args_hardened += '-ftrivial-auto-var-init=zero'
endif

c_args_warnings = [
  '-Wno-unused-parameter',
  '-Wno-gnu-zero-variadic-macro-arguments',
]

argp = subproject('argp').get_variable('argp_dep')

srcs = []
c_defines = []

fadec = subproject('fadec').get_variable('fadec').as_link_whole()
srcs += files(
  'src/main.c',
  'src/flags.c',
  'src/verify.c',
  'src/ht.c',
)

executable(
  'flag-verify',
  srcs,
  c_args: c_args_hardened + c_args_warnings + c_defines,
  dependencies: [argp, fadec],
  install: true,
)
