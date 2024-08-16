project('tetris', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

raylib = dependency('raylib')

exe = executable('tetris', 'tetris.c', dependencies: raylib)

test('basic', exe)
