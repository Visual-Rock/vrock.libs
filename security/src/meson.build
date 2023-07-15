src = [
    'random.ixx',
    'encryption/encryption.ixx',
    'encryption/aes.ixx',
    'encryption/rc4.ixx'
]

deps = []
deps += dependency('cryptopp', 'libcryptopp', required: true)
deps += vrockutils_dep

securitylib = static_library('vrocksecurity', src, dependencies: deps)

vrocksecurity_dep = declare_dependency(
    include_directories: include_directories('.'),
    sources: src,
    dependencies: deps
)

set_variable('vrocksecurity_dep', vrocksecurity_dep)

to_doc += {'name': 'security', 'files': src, 'pages': ['ByteArray'] }