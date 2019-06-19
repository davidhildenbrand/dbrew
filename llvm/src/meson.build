
sources = [
  'llengine.c',
  'llfunction.c',
  'support.cc',
]

dbrewllvm_includes = include_directories('.', '../include')

libdbrewllvm_lib = static_library('dbrewllvm', sources,
                                  include_directories: dbrewllvm_includes,
                                  dependencies: [libllvm, librellume],
                                  c_args: dbrewllvm_flags,
                                  cpp_args: dbrewllvm_flags,
                                  install: true)
