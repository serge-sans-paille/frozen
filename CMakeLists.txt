cmake_minimum_required(VERSION 3.8)
get_directory_property(is_subproject PARENT_DIRECTORY)

project(frozen VERSION 1.0.0 LANGUAGES CXX)
include(CTest)
include(CMakePackageConfigHelpers)
include(CMakeDependentOption)

CMAKE_DEPENDENT_OPTION(frozen.tests
  "Build the frozen tests and integrate with ctest"
  "${BUILD_TESTING}" "${is_subproject}" OFF)

CMAKE_DEPENDENT_OPTION(frozen.benchmark
  "Build the frozen benchmark"
  OFF "${is_subproject}" OFF)

add_library(frozen INTERFACE)
add_library(frozen::frozen ALIAS frozen)

target_compile_features(frozen INTERFACE cxx_std_14)

target_include_directories(frozen INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  $<INSTALL_INTERFACE:include>)

add_subdirectory(include/frozen)

if(frozen.tests)
  add_subdirectory(tests)
  add_subdirectory(examples)
endif()

if(frozen.benchmark)
  add_subdirectory(benchmarks)
endif()

install(TARGETS frozen EXPORT frozenConfig)

install(DIRECTORY include/
  DESTINATION include
  FILES_MATCHING PATTERN "*.h"
  PERMISSIONS OWNER_READ GROUP_READ)

install(EXPORT frozenConfig
  FILE frozenConfig.cmake
  NAMESPACE frozen
  DESTINATION share
  PERMISSIONS OWNER_READ GROUP_READ)

write_basic_package_version_file("frozenConfigVersion.cmake"
  VERSION ${frozen_VERSION}
  COMPATIBILITY SameMajorVersion)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/frozenConfigVersion.cmake"
  DESTINATION share
  PERMISSIONS OWNER_READ GROUP_READ)
