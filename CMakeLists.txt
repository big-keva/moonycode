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

add_custom_command(OUTPUT stat/trigraph.cpp
	COMMAND	echo "namespace codepages"               >  stat/trigraph.cpp
	COMMAND	echo "{"                                 >> stat/trigraph.cpp
	COMMAND echo "  unsigned char rus_trigraph[] ="  >> stat/trigraph.cpp
	COMMAND echo "  {"                               >> stat/trigraph.cpp
	COMMAND tritree ${moonycode_SOURCE_DIR}/text/kondrashov.txt | xxd -i >> stat/trigraph.cpp
	COMMAND echo "  }\;"                             >> stat/trigraph.cpp
	COMMAND echo "}"                                 >> stat/trigraph.cpp
	DEPENDS tritree ${moonycode_SOURCE_DIR}/text/kondrashov.txt stat )

add_custom_target(stat ALL
	COMMAND ${CMAKE_COMMAND} -E make_directory stat)

add_library(moonycode STATIC
	chartype.cpp
	cpdetect.cpp

	stat/trigraph.cpp

	xlat/utf16_lo.cpp
	xlat/utf16_up.cpp
	xlat/utf16win.cpp
	xlat/one_one.cpp
	xlat/win_dos.cpp
	xlat/win_iso.cpp
	xlat/win_koi.cpp
	xlat/win_mac.cpp)

add_executable(tritree
	util/tritree.cpp
	chartype.cpp)

install(TARGETS moonycode DESTINATION lib)
install(
  FILES
    chartype.h
    codes.h
  DESTINATION include/moonycode
)
