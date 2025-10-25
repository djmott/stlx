# FindCLANG_TIDY.cmake
# Find clang-tidy static analysis tool

find_program(CLANG_TIDY_EXECUTABLE
    NAMES clang-tidy
    DOC "clang-tidy executable"
)

if(CLANG_TIDY_EXECUTABLE)
    set(CLANG_TIDY_FOUND TRUE)
    message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXECUTABLE}")
else()
    set(CLANG_TIDY_FOUND FALSE)
    message(STATUS "clang-tidy not found - static analysis will be skipped")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CLANG_TIDY
    FOUND_VAR CLANG_TIDY_FOUND
    REQUIRED_VARS CLANG_TIDY_EXECUTABLE
)

mark_as_advanced(CLANG_TIDY_EXECUTABLE)