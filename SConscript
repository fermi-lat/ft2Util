# -*- python -*-
# $Id$
# Authors: Andrea Tramacere <tramacer@slac.stanford.edu>
# Version: ft2Util-01-02-32

Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

libEnv.Tool('ft2UtilLib', depsOnly = 1)
ft2UtilLib = libEnv.StaticLibrary('ft2Util', listFiles(['src/*.cxx']))

progEnv.Tool('ft2UtilLib')

makeFT2Entries = progEnv.Program('makeFT2Entries',
                                 listFiles(['src/apps/main.cxx']))
mergeFT2Entries = progEnv.Program('mergeFT2Entries',
                                  listFiles(['src/apps/mergeft2.cxx']))


progEnv.Tool('registerTargets', package = 'ft2Util',
             staticLibraryCxts = [[ft2UtilLib, libEnv]],
             testAppCxts = [[makeFT2Entries, progEnv],
                            [mergeFT2Entries, progEnv]],
             includes = listFiles(['ft2Util/*.h']))

