set(minimum_msvc_version 19.14)
set(minimum_gcc_version 7.0)
set(minimum_clang_version 4.0)
set(minimum_appleclang_version 6.0)

set(cxx_17_supported OFF)

if(MSVC AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${minimum_msvc_version})
    set(cxx_17_supported ON)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${minimum_gcc_version})
    set(cxx_17_supported ON)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${minimum_clang_version})
    set(cxx_17_supported ON)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${minimum_appleclang_version})
    set(cxx_17_supported ON)
endif()
