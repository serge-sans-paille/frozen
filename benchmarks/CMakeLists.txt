find_package(benchmark REQUIRED)

add_executable(frozen.benchmark "")
target_link_libraries(frozen.benchmark PUBLIC
  frozen::frozen
  benchmark::benchmark)

target_sources(frozen.benchmark PRIVATE
   ${CMAKE_CURRENT_LIST_DIR}/bench_int_set.cpp
   ${CMAKE_CURRENT_LIST_DIR}/bench_main.cpp
   ${CMAKE_CURRENT_LIST_DIR}/bench_str_set.cpp
   $<$<COMPILE_FEATURES:cxx_std_17>:
     ${CMAKE_CURRENT_LIST_DIR}/bench_str_search.cpp>)
