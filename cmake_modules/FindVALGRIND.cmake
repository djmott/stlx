# FindVALGRIND.cmake
# Find valgrind dynamic analysis tool

find_program(VALGRIND_EXECUTABLE
    NAMES valgrind
    DOC "valgrind executable"
)

if(VALGRIND_EXECUTABLE)
    set(VALGRIND_FOUND TRUE)
    message(STATUS "Found valgrind: ${VALGRIND_EXECUTABLE}")
else()
    set(VALGRIND_FOUND FALSE)
    message(STATUS "valgrind not found - dynamic analysis will be skipped")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(VALGRIND
    FOUND_VAR VALGRIND_FOUND
    REQUIRED_VARS VALGRIND_EXECUTABLE
)

mark_as_advanced(VALGRIND_EXECUTABLE)