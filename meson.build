project('vrocklibs', 'cpp', version: '0.0.1', default_options : ['cpp_std=c++20'])

src = []
examples = []
deps = []

if get_option('utils')
    subdir('utils')
endif

libs = library('vrocklibs', src, dependencies: deps)

if get_option('examples')
    foreach example_file : examples
        parts = example_file.split('/')
        file = parts[parts.length() - 1]
        executable(file.replace('.cpp', ''), example_file)
    endforeach
endif