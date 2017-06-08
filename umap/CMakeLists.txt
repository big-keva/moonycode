if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_executable(codestest
  codes.test.cpp
)
target_link_libraries(codestest
  tiny_codepages
)