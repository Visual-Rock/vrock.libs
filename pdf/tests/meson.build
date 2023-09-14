test_src = [
    'ByteArray.test.cpp'
]

gtest_proj = subproject('gtest')
gtest_dep = gtest_proj.get_variable('gtest_main_dep')


tests = executable(
    'utils tests', test_src,
    dependencies: [ gtest_dep, vrockutils_dep]
)

test('utils tests', tests)
