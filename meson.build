project('tetris', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

raylib = dependency('raylib', version: '>=4.5.0')

src = ['src/main.c', 'src/tetris.c']
exe = executable('tetris', sources: src, dependencies: raylib)

check = dependency('check', version: '>=0.15.2')

testSrc = ['test/main.c']
testExe = executable('tests', sources: testSrc, dependencies: check)
test('tests', testExe)
