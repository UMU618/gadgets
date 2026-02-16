boost_dep = dependency(
    'boost',
    modules: ['algorithm', 'filesystem', 'nowide', 'program_options'],
    required: true,
)
fmt_dep = dependency('fmt', required: true)

count_lines_deps = [boost_dep, fmt_dep]

count_lines = executable(
    'count_lines',
    '../../src/umutech/count_lines/count_lines.cpp',
    dependencies: count_lines_deps,
    install: true,
    build_by_default: true,
    install_dir: executable_output_dir,
)
