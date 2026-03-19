find_path(MoonyCode_INCLUDE
    NAMES codes.h chartype.h
    PATHS /usr/local/include/moonycode /usr/include/moonycode)

find_library(MoonyCode_LIB
    NAMES moonycode
    PATHS /usr/local/lib /usr/lib)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(MoonyCode
    REQUIRED_VARS MoonyCode_LIB MoonyCode_INCLUDE)

mark_as_advanced(MoonyCode_INCLUDE MoonyCode_LIB)
