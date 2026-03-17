find_path(MoonyCode_INCLUDE_DIR
    NAMES codes.h chartype.h
    PATHS /usr/local/include/moonycode /usr/include/moonycode)

find_library(MoonyCode_LIBRARY
    NAMES moonycode
    PATHS /usr/local/lib /usr/lib)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(MoonyCode
    REQUIRED_VARS MoonyCode_LIBRARY MoonyCode_INCLUDE_DIR)

mark_as_advanced(MoonyCode_INCLUDE_DIR MoonyCode_LIBRARY)
