project('vrocklibs', 'cpp', version: '0.0.1', default_options : ['cpp_std=c++23'])

to_doc = []

subdir('utils')

if get_option('security')
    subdir('security')
endif

if get_option('ui')
    subdir('ui')
endif

if get_option('pdf')
    subdir('pdf')
endif

if get_option('docs')
    subdir('docs')
endif