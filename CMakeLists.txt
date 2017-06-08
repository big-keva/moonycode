cmake_minimum_required(VERSION 3.2)
project(tiny_codepages)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)
if(MSVC)
  add_compile_options(-MP)
else()
  add_compile_options(-W -Wall -Wextra)
endif()

add_library(tiny_codepages STATIC
  chartype.cpp
  utf16_lo.cpp
  utf16_up.cpp
  utf16win.cpp
  win_dos.cpp
  win_iso.cpp
  win_koi.cpp
  win_mac.cpp
)
install(TARGETS tiny_codepages DESTINATION lib)
install(FILES
  chartype.h
  codes.h
  DESTINATION include/libcodes
)

add_subdirectory(test)
#add_subdirectory(umap)