src = [
    'src/main.cpp'
]

utilslib = library('vrockui', src)

vrockui_dep = declare_dependency(
    include_directories: include_directories('./include'),
    sources: src,
)

set_variable('vrockui_dep', vrockui_dep)

