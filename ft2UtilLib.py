#$Id$
def generate(env, **kw):
    if not kw.get('depsOnly', 0):
        env.Tool('addLibrary', library = ['ft2Util'])
    env.Tool('digiRootDataLib')
    env.Tool('commonRootDataLib')
    env.Tool('astroLib')
    env.Tool('fitsGenLib')
    env.Tool('facilitiesLib')
    env.Tool('addLibrary', library = env['clhepLibs'])
    env.Tool('addLibrary', library = env['cfitsioLibs'])

def exists(env):
    return 1
