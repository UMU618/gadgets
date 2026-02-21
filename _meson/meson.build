project(
    'gadgets',
    'cpp',
    version: '0.1',
    meson_version : '>= 1.10.1',
    default_options: [
        'cpp_std=c++20',
        'default_library=shared',
        'warning_level=3',
    ],
)

cxx = meson.get_compiler('cpp')

vcpkg = find_program('vcpkg', required: true)
# pkgconfig = find_program('pkgconf', required : true)

executable_output_dir = join_paths(meson.project_source_root(), 'bin')
if get_option('buildtype').startswith('debug')
    executable_output_dir = join_paths(executable_output_dir, 'Debug')
else
    executable_output_dir = join_paths(executable_output_dir, 'Release')
endif

subdir('count_lines')
subdir('process_dump')
subdir('sln2slnx')
