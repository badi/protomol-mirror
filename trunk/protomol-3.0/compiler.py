opts.AddOptions(
    EnumOption('compiler', 'Select compiler', 'default',
               allowed_values = ('default', 'gnu', 'intel', 'mingw', 'msvc',
                                 'linux-mingw', 'aix', 'posix', 'hp', 'sgi',
                                 'sun')))

compiler = env.get('compiler', 0)


# Select compiler
if compiler:
    if compiler == 'gnu':
        Tool('gcc')(env)
        Tool('g++')(env)

    elif compiler == 'intel':
        Tool('intelc')(env)
        env['ENV']['INTEL_LICENSE_FILE'] = os.environ.get("INTEL_LICENSE_FILE",
                                                          '')
    elif compiler == 'linux-mingw':
        env.Replace(CC = 'i586-mingw32msvc-gcc')
        env.Replace(CXX = 'i586-mingw32msvc-g++')
        env.Replace(RANLIB = 'i586-mingw32msvc-ranlib')
        env.Replace(PROGSUFFIX = '.exe')

    elif compiler == 'posix':
        Tool('cc')(env)
        Tool('cxx')(env)
        Tool('link')(env)
        Tool('ar')(env)
        Tool('as')(env)

    elif compiler in Split('hp sgi sun aix'):
        Tool(compiler + 'cc')(env)
        Tool(compiler + 'c++')(env)
        Tool(compiler + 'link')(env)

        if compiler in Split('sgi sun'):
            Tool(compiler + 'ar')(env)

    elif compiler != 'default':
        Tool(compiler)(env)

print "Compiler: " + env['CC']
print "Platform: " + env['PLATFORM']


# Options
if env['CC'] == 'cl':
    env.Append(CCFLAGS = '-EHsc')


# Debug flags
if debug:
    if env['CC'] == 'cl':
        env.Append(CCFLAGS = '-W1 /MDd')
    elif env['CC'] == 'gcc':
        env.Append(CCFLAGS = '-g -Wall')
        if strict: env.Append(CCFLAGS = '-Werror')

    env.Append(CPPDEFINES = ['DEBUG'])


# Optimizations
if optimize:
    if env['CC'] in ['icc', 'icpc']:
        env.Append(CCFLAGS = '-O -finline-functions -funroll-loops')
    elif env['CC'] == 'gcc':
        env.Append(CCFLAGS =
                   '-O9 -ffast-math -finline-functions -funroll-loops')
