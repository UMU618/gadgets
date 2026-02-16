boost_dep = dependency(
    'boost',
    modules: ['algorithm', 'filesystem', 'nowide'],
    required: true,
)
fmt_dep = dependency('fmt', required: true)

process_dump_deps = [boost_dep, fmt_dep]

process_dump = executable(
    'process_dump',
    '../../src/umutech/process_dump/process_dump.cpp',
    dependencies: process_dump_deps,
    install: true,
    build_by_default: true,
    install_dir: executable_output_dir,
)
