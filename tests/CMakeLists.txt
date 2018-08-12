add_executable(frozen.tests "")
target_link_libraries(frozen.tests PUBLIC frozen::frozen)
target_sources(frozen.tests PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/bench.hpp
  ${CMAKE_CURRENT_LIST_DIR}/catch.hpp
  ${CMAKE_CURRENT_LIST_DIR}/test_algorithms.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test_main.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test_map.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test_rand.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test_set.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test_str.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test_str_set.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test_unordered_map.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test_unordered_map_str.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test_unordered_set.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test_unordered_str_set.cpp)

string( CONCAT generator
  "$<$<CXX_COMPILER_ID:MSVC>:/W3;/WX;>"
  "$<$<OR:$<CXX_COMPILER_ID:GNU>"
        ",$<CXX_COMPILER_ID:Clang>"
        ",$<CXX_COMPILER_ID:AppleClang>>"
   ":-Wall-Wextra;-Wpedantic;-Werror;-Wshadow;"
    "$<$<BOOL:${frozen.coverage}>:--coverage>>"
  "$<$<CXX_COMPILER_ID:Intel>:"
    "$<$<PLATFORM_ID:Windows>:/Wall;>"
    "$<$<NOT:$<PLATFORM_ID:Windows>>:-Wall;>>")

add_compile_options(frozen.tests PUBLIC ${generator})
add_test(NAME frozen.tests COMMAND frozen.tests)
 
