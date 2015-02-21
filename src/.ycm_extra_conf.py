import os

FLAGS = [
    '-Wall',
    '-Wextra',
    '-Werror',
    '-std=c++11',
    '-stdlib=libc++',

    '-I', os.path.dirname(os.path.abspath(__file__))
]


def patch_flags_for_this_os():
    global FLAGS

    platform = os.uname()[0]

    if 'Darwin' == platform:
        FLAGS.append('-isystem')
        FLAGS.append('/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1')
        FLAGS.extend(['-I', '/usr/local/Cellar/protobuf/2.6.1/include'])
    else:
        raise RuntimeError('Unknow platform!')


def FlagsForFile(filename, **kwars):
    patch_flags_for_this_os()

    return {
        'flags': FLAGS,
        'do_cache': True
    }
