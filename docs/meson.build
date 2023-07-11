files = []
toc = '.. toctree::\n   :glob:\n   :maxdepth: 2\n   :caption: General\n\n   pages/howtouse\n\n'
toc_base = '.. toctree::\n   :glob:\n   :maxdepth: 2\n   :caption: '
foreach module : to_doc
    name = module.get('name')
    src_files = module.get('files')
    pages = module.get('pages')

    foreach f : src_files
        files += ''.join( '../', name, '/src/', f )
    endforeach

    t = ''.join( toc_base, name, '\n\n' )
    foreach p : pages
        t = ''.join(t, '   pages/', name, '/', p, '\n')
    endforeach
    toc += t
    toc += '\n'
endforeach
toc += '.. toctree::\n   :glob:\n   :maxdepth: 2\n   :caption: Reference\n\n   out/namespacelist\n   out/filelist\n\n'
message(toc)
foreach module : to_doc
    name = module.get('name')
    pages = module.get('pages')
    foreach p : pages
        conf_data = configuration_data()
        conf_data.set('toc', toc)
        doc_file = configure_file(input: ''.join('pages/', name, '/', p, '.rst.in'),
            output : ''.join(p, '.rst'),
            configuration : conf_data
        )
    endforeach
endforeach

doxygen = find_program('doxygen', required : false)
apidoc = find_program('breathe-apidoc', required : false)
sphinx_build = find_program('sphinx-build', required : false)

conf_data = configuration_data()
conf_data.set('toc', ' '.join(toc))
index_file = configure_file(input: 'index.rst.in',
    output : 'index.rst',
    configuration : conf_data
)

conf_data = configuration_data()
conf_data.set('FILES', ' '.join(files)) # headers from the src meson.build
doxy_file = configure_file(input: 'Doxyfile.in',
    output : 'Doxyfile',
    configuration : conf_data
)

python = find_program('python3', 'python')
copy = files('copy.py')
index = run_command(
    python,
    copy,
    index_file, 
    join_paths('docs', 'index.rst')
)

if doxygen.found()
    # generate doxygen xml files
    doxygen_xml = custom_target('doxygen-xml', output: 'xml', command: [ doxygen, doxy_file])

    if apidoc.found()
        apidoc_gen = custom_target('gen-breathe-apidoc',
            output: 'apidoc',
            command: [ apidoc, '-o', '../docs/out', '-m', '-f', '../docs/xml'],
            depends: [ doxygen_xml ]
        )

        if sphinx_build.found()
            custom_target('docs',
                output: '.',
                command: [ sphinx_build, '-b', 'html', meson.source_root() + '/docs/', meson.source_root() + '/docs/html'],
                install: false,
                depends: [ apidoc_gen ],
                 build_always_stale: true,
                input: index_file
            )
        else
            warning('docs can not be build without sphinx-build')
        endif
    else
        warning('docs can not be build without breathe-apidoc')
    endif
else
    warning('docs can not be build without doxygen')
endif