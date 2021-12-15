project('frozen',
  'cpp',
  version : '1.1.1',
  license : 'Apache-2.0',
)

install_headers(
  'include/frozen/algorithm.h',
  'include/frozen/map.h',
  'include/frozen/random.h',
  'include/frozen/set.h',
  'include/frozen/string.h',
  'include/frozen/unordered_map.h',
  'include/frozen/unordered_set.h',
  subdir : 'frozen',
)

install_headers(
  'include/frozen/bits/algorithms.h',
  'include/frozen/bits/basic_types.h',
  'include/frozen/bits/constexpr_assert.h',
  'include/frozen/bits/defines.h',
  'include/frozen/bits/elsa.h',
  'include/frozen/bits/elsa_std.h',
  'include/frozen/bits/exceptions.h',
  'include/frozen/bits/hash_string.h',
  'include/frozen/bits/pmh.h',
  'include/frozen/bits/version.h',
  subdir : 'frozen/bits',
)

pkgc = import('pkgconfig')
pkgc.generate(name: 'frozen',
  version: meson.project_version(),
  description: '0 cost initialization for immutable containers, fixed-size containers, and various algorithms.'
)

incdir = include_directories('include')

frozentests = executable(
  'frozen.tests',
  'tests/test_algorithms.cpp',
  'tests/test_elsa_std.cpp',
  'tests/test_main.cpp',
  'tests/test_map.cpp',
  'tests/test_rand.cpp',
  'tests/test_set.cpp',
  'tests/test_str.cpp',
  'tests/test_str_set.cpp',
  'tests/test_unordered_map.cpp',
  'tests/test_unordered_map_str.cpp',
  #'tests/test_unordered_set.cpp',
  'tests/test_unordered_str_set.cpp',
  include_directories : incdir
)

test('frozentests', frozentests)

frozen_dep = declare_dependency(
  include_directories : incdir
)
