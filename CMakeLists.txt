cmake_minimum_required(VERSION 3.5)
project(moonycode)

option(INSTALL_MOONYCODE "Elable moonycode installation" OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if(MSVC)
  add_compile_options(-MP)
else()
  add_compile_options(-pedantic -mfpmath=sse
    -W -Wall -Wextra -Wcast-align -Wfloat-equal -Wpointer-arith -Wformat-security
    -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long
    -Woverloaded-virtual -Wno-suggest-attribute=format)
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-fno-stack-protector -D_FORTIFY_SOURCE=0 -fno-builtin)
  else()
    add_compile_options(-fstack-protector-all -g)
  endif()
endif()

add_compile_options(-fPIC)

# add_subdirectory(test)

add_custom_command(OUTPUT stat/trigraph.cpp
  COMMAND moonycode-tritree ${moonycode_SOURCE_DIR}/text/kondrashov.txt rus_trigraph codepages > stat/trigraph.cpp
  DEPENDS moonycode-tritree ${moonycode_SOURCE_DIR}/text/kondrashov.txt stat )

add_custom_target(stat ALL
  COMMAND ${CMAKE_COMMAND} -E make_directory stat)

add_custom_command(OUTPUT stat/chartype.cpp
  COMMAND moonycode-mktypes ${moonycode_SOURCE_DIR}/UnicodeData.txt charType codepages > stat/chartype.cpp
  DEPENDS moonycode-mktypes ${moonycode_SOURCE_DIR}/UnicodeData.txt stat )

add_library(moonycode STATIC
  cpdetect.cpp

  stat/chartype.cpp
  stat/trigraph.cpp

  xlat/utf16_lo.cpp
  xlat/utf16_up.cpp
  xlat/utf16win.cpp
  xlat/one_one.cpp
  xlat/win_dos.cpp
  xlat/win_iso.cpp
  xlat/win_koi.cpp
  xlat/win_mac.cpp)

add_executable(moonycode-tritree
  util/tritree.cpp
  stat/chartype.cpp)

add_executable(moonycode-mktypes
  util/mktypes.cpp)

if(INSTALL_MOONYCODE)
  install(TARGETS moonycode DESTINATION lib)
  install(FILES
    chartype.h
    codes.h
  DESTINATION include/moonycode)
endif()
