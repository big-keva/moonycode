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
# build shared library
#
add_library(moonycode-shared SHARED
  $<TARGET_OBJECTS:moonycode-objset>)

set_target_properties(moonycode-shared PROPERTIES 
  OUTPUT_NAME "moonycode")

#
# build static library
#
add_library(moonycode-static STATIC
  $<TARGET_OBJECTS:moonycode-objset>)

set_target_properties(moonycode-static PROPERTIES
  OUTPUT_NAME "${STATIC_LIB_PREFIX}moonycode"
  CLEAN_DIRECT_OUTPUT 1)

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

#
# install section
#
install(
  TARGETS moonycode-shared
  LIBRARY DESTINATION lib COMPONENT Rtm
  RUNTIME DESTINATION bin COMPONENT Rtm)

install(
  TARGETS moonycode-static
  ARCHIVE DESTINATION lib COMPONENT Dev)

install(
  FILES chartype.h codes.h 
  DESTINATION include/moonycode COMPONENT Dev)

install(
  FILES cmake/MoonyCodeConfig.cmake 
  DESTINATION lib/cmake/MoonyCode COMPONENT Dev)

#
# CPack
#
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" PACKAGE_VERSION_RAW)
string(STRIP "${PACKAGE_VERSION_RAW}" CPACK_PACKAGE_VERSION)

set(CPACK_PACKAGE_NAME "moonycode")
set(CPACK_PACKAGE_CONTACT "Andrew Kovalenko aka Keva <keva@rambler.ru>")

set(CPACK_COMPONENTS_ALL Rtm Dev)

#
# enable generators
#
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_RPM_COMPONENT_INSTALL ON)

if(WIN32)
  set(CPACK_GENERATOR "ZIP")
else()
  set(CPACK_GENERATOR "DEB;RPM;TGZ")
endif()

#
# DEB specific
#
# DEB specific
set(CPACK_DEB_COMPONENT_INSTALL       ON)

set(CPACK_DEBIAN_DEV_PACKAGE_DEPENDS  "moonycode (= ${CPACK_PACKAGE_VERSION})")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")

set(CPACK_DEBIAN_RTM_PACKAGE_NAME     "moonycode") 
set(CPACK_DEBIAN_DEV_PACKAGE_NAME     "moonycode-dev")

set(CPACK_DEBIAN_FILE_NAME            "DEB-DEFAULT")

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS    ON)

#
# RPM specific
#
set(CPACK_RPM_DEV_PACKAGE_REQUIRES    "moonycode = ${CPACK_PACKAGE_VERSION}")
set(CPACK_RPM_PACKAGE_ARCHITECTURE    "x86_64")

set(CPACK_RPM_PACKAGE_RELEASE         "1")

set(CPACK_RPM_RTM_PACKAGE_NAME        "moonycode")
set(CPACK_RPM_DEV_PACKAGE_NAME        "moonycode-devel")

set(CPACK_RPM_FILE_NAME               "RPM-DEFAULT")

include(CPack)
