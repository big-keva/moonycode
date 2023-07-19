cmake_minimum_required(VERSION 3.1)
project(moonycode)

set(CMAKE_C_STANDARD 14)
set(CMAKE_CXX_STANDARD 14)
if(MSVC)
  add_compile_options(-MP)
else()
  add_compile_options(-W -Wall -Wextra)
  add_compile_options(-mfpmath=sse -fstack-protector-all -g -W -Wall -Wextra -Wcast-align -pedantic -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long -Woverloaded-virtual -Wno-suggest-attribute=format)
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
  win_mac.cpp)

install(TARGETS moonycode DESTINATION lib)
install(
  FILES
    chartype.h
    codes.h
  DESTINATION include/moonycode
)
