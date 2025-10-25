# FindCLANG_FORMAT.cmake
# Find clang-format code formatting tool

find_program(CLANG_FORMAT_EXECUTABLE
    NAMES clang-format
    DOC "clang-format executable"
)

if(CLANG_FORMAT_EXECUTABLE)
    set(CLANG_FORMAT_FOUND TRUE)
    message(STATUS "Found clang-format: ${CLANG_FORMAT_EXECUTABLE}")
else()
    set(CLANG_FORMAT_FOUND FALSE)
    message(STATUS "clang-format not found - code formatting will be skipped")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CLANG_FORMAT
    FOUND_VAR CLANG_FORMAT_FOUND
    REQUIRED_VARS CLANG_FORMAT_EXECUTABLE
)

mark_as_advanced(CLANG_FORMAT_EXECUTABLE)