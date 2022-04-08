cmake_minimum_required(VERSION 3.1)
project(moonycode)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
if(MSVC)
  add_compile_options(-MP)
else()
  add_compile_options(-W -Wall -Wextra)
endif()

add_subdirectory(test)

add_library(moonycode STATIC
  chartype.cpp
  utf16_lo.cpp
  utf16_up.cpp
  utf16win.cpp
  win_dos.cpp
  win_iso.cpp
  win_koi.cpp
  win_mac.cpp
)
install(TARGETS moonycode DESTINATION lib)
install(
  FILES
    chartype.h
    codes.h
  DESTINATION include/moonycode
)
