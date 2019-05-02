
libllvm = dependency('llvm', version: ['>=4', '<8'])
dbrewllvm_flags = ['-DLL_LLVM_MAJOR='+libllvm.version().split('.')[0]]

rellume = subproject('rellume')
librellume = rellume.get_variable('librellume')

subdir('src')
subdir('benchmark')
subdir('examples')
subdir('test')

