src = [
    'ByteArray.ixx',
    'List.ixx',
    'Timer.ixx',
    'FutureHelper.ixx'
]

utilslib = static_library('vrockutils', src)

vrockutils_dep = declare_dependency(
    include_directories: include_directories('.'),
    sources: src,
)

set_variable('vrockutils_dep', vrockutils_dep)

to_doc += {'name': 'utils', 'files': src, 'pages': ['ByteArray'] }