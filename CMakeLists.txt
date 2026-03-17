cmake_minimum_required(VERSION 3.5)
project(moonycode)

#
# check configuration
#
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

if(MSVC AND NOT DEFINED STATIC_LIB_PREFIX)
  set(STATIC_LIB_PREFIX "lib")
endif()

#
# DEVELOPER_MODE enforces building all the targets in CMakeLists;
#
# if !defined, try define it according to the building environment:
#   CI - no default targets;
#   CLION_IDE - all targets enabled.
#
if(NOT DEFINED DEVELOPER_MODE)
  message(STATUS "'DEVELOPER_MODE' it not set, try detect the build environment...")

  if(DEFINED ENV{CLION_IDE})
    message(STATUS "Clion build environment detected")

    set(DEVELOPER_MODE TRUE CACHE BOOL "" FORCE)
  else()
    set(DEVELOPER_MODE FALSE CACHE BOOL "" FORCE)
  endif()
endif()

if(NOT DEFINED BUILD_SHARED)
  set(BUILD_SHARED ${DEVELOPER_MODE} CACHE BOOL "" FORCE)
endif()

if(NOT DEFINED BUILD_STATIC)
  set(BUILD_STATIC ${DEVELOPER_MODE} CACHE BOOL "" FORCE)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if(MSVC)
  add_compile_options(-MP)
else()
  add_compile_options(-fPIC)

  add_compile_options(-pedantic -mfpmath=sse
    -W -Wall -Wextra -Wcast-align -Wfloat-equal -Wpointer-arith -Wformat-security
    -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long
    -Woverloaded-virtual -Wno-suggest-attribute=format)

  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-fno-stack-protector -fno-builtin)
  else()
    add_compile_options(-fstack-protector-all -g)
  endif()
endif()

#
# building tools for dictionaries and tables
#
if(BUILD_SHARED OR BUILD_STATIC)
  #
  # objset for libraries
  #
  add_library(moonycode-objset OBJECT
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

  set_target_properties(moonycode-objset PROPERTIES
      POSITION_INDEPENDENT_CODE ON)

  #
  # trigraph dictionary folder
  #
  add_custom_target(stat ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory stat)

  #
  # trigraph dictionary convertor to compileable source
  #
  add_custom_command(OUTPUT stat/trigraph.cpp
    COMMAND moonycode-tritree ${moonycode_SOURCE_DIR}/text/kondrashov.txt rus_trigraph codepages > stat/trigraph.cpp
    DEPENDS moonycode-tritree ${moonycode_SOURCE_DIR}/text/kondrashov.txt stat )

  #
  # character type table building programme
  #
  add_custom_command(OUTPUT stat/chartype.cpp
    COMMAND moonycode-mktypes ${moonycode_SOURCE_DIR}/UnicodeData.txt charType codepages > stat/chartype.cpp
    DEPENDS moonycode-mktypes ${moonycode_SOURCE_DIR}/UnicodeData.txt stat )

  add_executable(moonycode-tritree util/tritree.cpp stat/chartype.cpp)
  add_executable(moonycode-mktypes util/mktypes.cpp)

  add_subdirectory(test)
endif()

#
# check if build shared library
#
if(BUILD_SHARED)
  add_library(moonycode-shared SHARED
    $<TARGET_OBJECTS:moonycode-objset>)

  set_target_properties(moonycode-shared PROPERTIES 
    OUTPUT_NAME "moonycode")

  #
  # install shared library
  #
  install(
    TARGETS moonycode-shared
    DESTINATION lib)
endif()

#
# check if build static library
#
if(BUILD_STATIC)
  add_library(moonycode-static STATIC
    $<TARGET_OBJECTS:moonycode-objset>)

  set_target_properties(moonycode-static PROPERTIES
    OUTPUT_NAME "${STATIC_LIB_PREFIX}moonycode"
    CLEAN_DIRECT_OUTPUT 1)

  #
  # install static library and sources
  #
  install(
    TARGETS moonycode-static
    DESTINATION lib)
  install(
    FILES chartype.h codes.h
    DESTINATION include/moonycode)
  install(FILES "cmake/MoonyCodeConfig.cmake" 
    DESTINATION "lib/cmake/MoonyCode")
endif()
