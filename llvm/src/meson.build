
sources = [
  'dbrew-backend.c',
  'llengine.c',
  'llfunction.c',
  'support.cc',
]

dbrewllvm_includes = include_directories('.', '../include')

libdbrewllvm_lib = static_library('dbrewllvm', sources,
                                  include_directories: dbrewllvm_includes,
                                  dependencies: [libllvm, dbrew_priv, librellume],
                                  c_args: dbrewllvm_flags,
                                  cpp_args: dbrewllvm_flags,
                                  install: true)
libdbrewllvm = declare_dependency(include_directories: dbrewllvm_includes,
                                  link_with: libdbrewllvm_lib,
                                  dependencies: [dbrew, libllvm, librellume])

