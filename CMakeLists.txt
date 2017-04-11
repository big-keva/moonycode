cmake_minimum_required(VERSION 2.8)

project(codepages)

add_definitions( -Wall -std=c++1y -pedantic )

set(SOURCE_LIB chartype.cpp  utf16_lo.cpp  utf16_up.cpp  utf16win.cpp  win_dos.cpp  win_iso.cpp  win_koi.cpp  win_mac.cpp)

add_library(codes STATIC ${SOURCE_LIB})
