boost_dep = dependency(
    'boost',
    modules: ['algorithm', 'filesystem', 'nowide'],
    required: true,
)

all_deps = [boost_dep]

sln2slnx = executable(
    'sln2slnx',
    '../../src/umutech/sln2slnx/sln2slnx.cpp',
    dependencies: all_deps,
    install: true,
    build_by_default: true,
    install_dir: executable_output_dir,
)
