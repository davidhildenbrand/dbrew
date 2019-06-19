
subdir('src')

dbreworiginllvm = declare_dependency(include_directories: include_directories('include'),
                                     link_with: dbreworiginllvm_lib,
                                     dependencies: [dbrew, dbrewllvm])

subdir('benchmark')
subdir('examples')
