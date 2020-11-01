set(cxx17_minimum_msvc_version 19.14)
set(cxx17_minimum_gcc_version 7.0)
set(cxx17_minimum_clang_version 4.0)
set(cxx17_minimum_appleclang_version 6.0)

set(cxx20_minimum_msvc_version 19.22)
set(cxx20_minimum_gcc_version 9.0)
set(cxx20_minimum_clang_version 7.0)
set(cxx20_minimum_appleclang_version 10.0)

set(cxx_17_supported OFF)

if(MSVC AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${cxx17_minimum_msvc_version})
  set(cxx_17_supported ON)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${cxx17_minimum_gcc_version})
  set(cxx_17_supported ON)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${cxx17_minimum_clang_version})
  set(cxx_17_supported ON)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${cxx17_minimum_appleclang_version})
  set(cxx_17_supported ON)
endif()

set(cxx_20_supported OFF)

if(MSVC AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${cxx20_minimum_msvc_version})
  set(cxx_20_supported ON)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${cxx20_minimum_gcc_version})
  set(cxx_20_supported ON)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${cxx20_minimum_clang_version})
  set(cxx_20_supported ON)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${cxx20_minimum_appleclang_version})
  set(cxx_20_supported ON)
endif()

if(cxx_20_supported)
  set(max_cxx_requirement "cxx_std_20")
elseif(cxx_17_supported)
  set(max_cxx_requirement "cxx_std_17")
else()
  set(max_cxx_requirement "cxx_std_14")
endif()
