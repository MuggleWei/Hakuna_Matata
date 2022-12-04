project('use_pkg', 'c')
foodep = dependency('foo')
executable('bob',
  'bob/bob.c',
  dependencies: foodep,
  install: true,
  install_rpath: '$ORIGIN/../lib')
