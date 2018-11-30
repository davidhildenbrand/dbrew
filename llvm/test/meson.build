
glib = dependency('glib-2.0', version: '>=2.36.0')
gtester = find_program('gtester')

test_correctness = executable('dbrewllvm_test_correctness',
                              files('correctness.c'),
                              dependencies: [glib, libdbrewllvm])

test('correctness', gtester, args: ['--verbose', '-k', test_correctness])

