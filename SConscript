# -*- python -*-
# $Id$
# Authors: Giacomo Vianello <giacomov@slac.stanford.edu>
# Version: ft2Util-02-00-05

Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

libEnv.Tool('ft2UtilLib', depsOnly = 1)
ft2UtilLib = libEnv.StaticLibrary('ft2Util', listFiles(['src/*.cxx']))

progEnv.Tool('ft2UtilLib')

makeFT2 = progEnv.Program('makeFT2', listFiles(['src/makeFT2.cpp']))
mergeFT2 = progEnv.Program('mergeFT2', listFiles(['src/mergeFT2.cpp']))

progEnv.Tool('registerTargets', package = 'ft2Util',
             staticLibraryCxts = [[ft2UtilLib, libEnv]],
             testAppCxts = [[makeFT2, progEnv], [mergeFT2, progEnv]],
             includes = listFiles(['ft2Util/*.h']),
	     data = listFiles(['data/*.rc']))

