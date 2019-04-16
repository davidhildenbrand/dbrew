
sources = [
  'llbasicblock.c',
  'lldecoder.c',
  'llengine.c',
  'llflags.c',
  'llfunction.c',
  'llinstruction-callret.c',
  'llinstruction-gp.c',
  'llinstruction-sse.c',
  'llinstruction-stack.c',
  'llinstruction.c',
  'lloperand.c',
  'llregfile.c',
  'llsupport.cc',
]

dbrewllvm_includes = include_directories('../include')

libdbrewllvm_lib = static_library('dbrewllvm', sources,
                                  include_directories: dbrewllvm_includes,
                                  dependencies: [libllvm, libdbrew_dep_priv],
                                  c_args: dbrewllvm_flags,
                                  cpp_args: dbrewllvm_flags)
libdbrewllvm = declare_dependency(include_directories: dbrewllvm_includes,
                                  link_with: libdbrewllvm_lib,
                                  dependencies: [libdbrew_dep])

