cmake_minimum_required(VERSION 3.5)
project(test-moonycode)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
if(MSVC)
  add_compile_options(-MP)
else()
  add_compile_options(-W -Wall -Wextra)
endif()

add_executable(test-moonycode
  codes.test.cpp
)

target_link_libraries(test-moonycode moonycode)
