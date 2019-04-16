
executable('stencil', files('stencil.c', 'stencil-kernels.c', 'timer.c'),
           dependencies: [libdbrewllvm],
           c_args: ['-mno-avx', '-O3', '-march=native', '-DSTENCIL_INTERLINES=80'],
           link_args: ['-Wl,--as-needed', '-Wl,-z,now', '-Wl,-z,relro'])

executable('exp', files('exp.c', 'timer.c'),
           dependencies: [libdbrewllvm],
           c_args: ['-mno-avx', '-O3', '-march=native'],
           link_args: ['-Wl,--as-needed', '-Wl,-z,now', '-Wl,-z,relro'])
