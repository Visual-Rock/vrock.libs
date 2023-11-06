test_src = [
    'main.cpp'
]

gtest_proj = subproject('gtest')
gtest_dep = gtest_proj.get_variable('gtest_main_dep')


tests = executable(
    'ui tests', test_src,
    dependencies: [ gtest_dep, vrockui_dep]
)

test('ui tests', tests)
