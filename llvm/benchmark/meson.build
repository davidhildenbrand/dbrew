libdrob = dependency('libdrob', version: ['>=0.0.1', '<0.1.0'])

executable('stencil', files('stencil.c', 'stencil-kernels.c', 'timer.c'),
           dependencies: [libdbrewllvm, libdrob],
           c_args: ['-mno-avx', '-O3', '-march=x86-64', '-DSTENCIL_INTERLINES=80'],
           link_args: ['-Wl,--as-needed', '-Wl,-z,now', '-Wl,-z,relro'])

executable('exp', files('exp.c', 'timer.c'),
           dependencies: [libdbrewllvm, libdrob],
           c_args: ['-mno-avx', '-O3', '-march=x86-64'],
           link_args: ['-Wl,--as-needed', '-Wl,-z,now', '-Wl,-z,relro'])
