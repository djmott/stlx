# FindGCOVR.cmake
# Find gcovr code coverage tool

find_program(GCOVR_EXECUTABLE
    NAMES gcovr
    DOC "gcovr executable"
)

if(GCOVR_EXECUTABLE)
    set(GCOVR_FOUND TRUE)
    message(STATUS "Found gcovr: ${GCOVR_EXECUTABLE}")
else()
    set(GCOVR_FOUND FALSE)
    message(STATUS "gcovr not found - code coverage reports will be skipped")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GCOVR
    FOUND_VAR GCOVR_FOUND
    REQUIRED_VARS GCOVR_EXECUTABLE
)

mark_as_advanced(GCOVR_EXECUTABLE)