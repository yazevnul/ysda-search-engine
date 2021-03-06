import os
import ycm_core

FLAGS = [
    '-std=c++14',

    '-Weverything',
    # disable annoying warnings
    '-Wno-c++98-compat-pedantic',
    '-Wno-exit-time-destructors',
    '-Wno-float-equal',
    '-Wno-global-constructors',
    '-Wno-missing-prototypes',
    '-Wno-missing-variable-declarations',
    '-Wno-padded',
    '-Wno-unused-macros',

    '-stdlib=libc++',

    '-I', os.path.dirname(os.path.abspath(__file__))
]


compilation_database_folder = os.path.abspath(
    os.path.join(os.path.dirname(os.path.abspath(__file__)), '..')
)

if os.path.exists(compilation_database_folder):
    database = ycm_core.CompilationDatabase(compilation_database_folder)
else:
    database = None

SOURCE_EXTENSIONS = ['.cpp', '.cc', '.c', ]


def get_directory_of_this_script():
    return os.path.dirname(os.path.abspath(__file__))


def make_relative_paths_in_flags_absolute(flags, working_directory):
    if not working_directory:
        return list(flags)

    new_flags = []
    make_next_absolute = False
    path_flags = ['-isystem', '-I', '-iquote', '--sysroot=']
    for flag in flags:
        new_flag = flag

        if make_next_absolute:
            make_next_absolute = False
            if not flag.startswith('/'):
                new_flag = os.path.join(working_directory, flag)

        for path_flag in path_flags:
            if flag == path_flag:
                make_next_absolute = True
                break

            if flag.startswith(path_flag):
                path = flag[len(path_flag):]
                new_flag = path_flag + os.path.join(working_directory, path)
                break

        if new_flag:
            new_flags.append(new_flag)

    return new_flags


def is_header_file(filename):
    extension = os.path.splitext(filename)[1]
    return extension in ['.h', '.hxx', '.hpp', '.hh']


def get_compilation_info_for_file(filename):
    # The compilation_commands.json file generated by CMake does not have entries
    # for header files. So we do our best by asking the db for flags for a
    # corresponding source file, if any. If one exists, the flags for that file
    # should be good enough.
    if is_header_file(filename):
        basename = os.path.splitext(filename)[0]
        for extension in SOURCE_EXTENSIONS:
            replacement_file = basename + extension
            if os.path.exists(replacement_file):
                compilation_info = database.GetCompilationInfoForFile(replacement_file)
                if compilation_info.compiler_flags_:
                    return compilation_info

        return None

    return database.GetCompilationInfoForFile(filename)


def patch_flags_for_this_os():
    global FLAGS

    platform = os.uname()[0]

    if 'Darwin' == platform:
        FLAGS.append('-isystem')
        FLAGS.append('/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1')
        FLAGS.extend(['-I', '/usr/local/Cellar/protobuf/2.6.1/include'])
    else:
        raise RuntimeError('Unknow platform!')


def fix_compilation_database_for_this_os(flags):
    '''
    For details see this issue https://github.com/Valloric/YouCompleteMe/issues/303
    For some reason libclang adds /usr/local/include as a search path for STL headers while it must
    be something like /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/../include/c++/v1
    on Mac OS X. So we will fix this issue manually.
    '''

    platform = os.uname()[0]
    if 'Darwin' == platform:
        found = False
        if '-I/usr/local/include' in flags:
            flags.remove('-I/usr/local/include')
            found = True
        elif '-I/usr/include' in flags:
            flags.remove('-I/usr/include')
            found = True

        if found:
            flags.append('-I/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1')
    else:
        raise RuntimeError('Unknown platform!')


def make_flags_without_database():
    relative_to = get_directory_of_this_script()
    patch_flags_for_this_os()
    final_flags = make_relative_paths_in_flags_absolute(FLAGS, relative_to)
    return final_flags


def FlagsForFile(filename, **kwargs):
    if database:
        # Bear in mind that compilation_info.compiler_flags_ does NOT return a
        # python list, but a "list-like" StringVec object
        compilation_info = get_compilation_info_for_file(filename)
        if compilation_info:
            final_flags = make_relative_paths_in_flags_absolute(
                compilation_info.compiler_flags_,
                compilation_info.compiler_working_dir_
            )
            fix_compilation_database_for_this_os(final_flags)
        else:
            final_flags = make_flags_without_database()
    else:
        final_flags = make_flags_without_database()

    return {
        'flags': final_flags,
        'do_cache': True
    }
