cmake_minimum_required(VERSION 3.5)
project(codepages)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
add_compile_options(-W -Wall -Wextra)

add_library(codes STATIC
  chartype.cpp
  utf16_lo.cpp
  utf16_up.cpp
  utf16win.cpp
  win_dos.cpp
  win_iso.cpp
  win_koi.cpp
  win_mac.cpp
)
