boost_dep = dependency(
    'boost',
    modules: ['algorithm', 'filesystem', 'nowide', 'program_options'],
    required: true,
)
fmt_dep = dependency('fmt', required: true)

all_deps = [boost_dep, fmt_dep]

count_lines = executable(
    'count_lines',
    '../../src/umutech/count_lines/count_lines.cpp',
    dependencies: all_deps,
    install: true,
    build_by_default: true,
    install_dir: executable_output_dir,
)
