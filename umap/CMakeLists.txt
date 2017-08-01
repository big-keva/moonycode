cmake_minimum_required(VERSION 3.5)
project(umap)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)
if(MSVC)
  add_compile_options(-MP)
else()
  add_compile_options(-W -Wall -Wextra)
endif()

add_executable(umap
  createUmap.cpp
)
