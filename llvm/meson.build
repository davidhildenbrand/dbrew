
libllvm = dependency('llvm', version: ['>=4', '<7'])
dbrewllvm_flags = ['-DLL_LLVM_MAJOR='+libllvm.version().split('.')[0]]

subdir('src')
subdir('benchmark')
subdir('examples')
subdir('test')

