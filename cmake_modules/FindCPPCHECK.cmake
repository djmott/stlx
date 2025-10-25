# FindCPPCHECK.cmake
# Find cppcheck static analysis tool

find_program(CPPCHECK_EXECUTABLE
    NAMES cppcheck
    DOC "cppcheck executable"
)

if(CPPCHECK_EXECUTABLE)
    set(CPPCHECK_FOUND TRUE)
    message(STATUS "Found cppcheck: ${CPPCHECK_EXECUTABLE}")
else()
    set(CPPCHECK_FOUND FALSE)
    message(STATUS "cppcheck not found - static analysis will be skipped")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CPPCHECK
    FOUND_VAR CPPCHECK_FOUND
    REQUIRED_VARS CPPCHECK_EXECUTABLE
)

mark_as_advanced(CPPCHECK_EXECUTABLE)