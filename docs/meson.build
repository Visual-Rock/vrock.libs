doxygen = find_program('doxygen', required : false)
sphinx_build = find_program('sphinx-build', required : false)
python = find_program('python3', 'python')

files = meson.current_source_dir() + '/../utils/src/include/'

if get_option('security')
    files += ' ' + meson.current_source_dir() + '/../security/src/'
endif

if get_option('ui')
    files += ' ' + meson.current_source_dir() + '/../ui/src/'
endif

if get_option('pdf')
    files += ' ' + meson.current_source_dir() + '/../pdf/src/'
endif

message(files)

conf_data = configuration_data()
conf_data.set('FILES', files)
conf_data.set('DOXYGEN_OUTPUT_DIR', meson.current_build_dir() + '/_build/')
conf_data.set('DOXYGEN_INDEX_FILE', meson.current_build_dir() + '/_build/xml/index.xml')

doxy_file = configure_file(input: 'Doxyfile.in', output: 'Doxyfile', configuration: conf_data)

copy_py = files('copy.py')
setup = run_command(
    python, copy_py, meson.current_source_dir() + '/', meson.current_build_dir() + '/', meson.current_build_dir() + '/_build/'
)
message(setup.stdout())

if doxygen.found()
    # generate doxygen xml files
    doxygen_target = custom_target('Doxygen', 
        output: 'index.xml',
        command: [ doxygen, doxy_file]
    )
    if sphinx_build.found()
        custom_target('Docs',
            output: 'html',
            command: [ sphinx_build, '-b', 'html', meson.current_build_dir(), meson.current_build_dir() + '/_build/html'],
            build_always_stale: true,
        )
    else
        warning('docs can not be build without sphinx-build')
    endif
else
    warning('docs can not be build without doxygen')
endif