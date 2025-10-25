# FindIWYU.cmake
# Find Include What You Use (IWYU) tool

find_program(IWYU_EXECUTABLE
    NAMES include-what-you-use iwyu
    DOC "Include What You Use executable"
)

# Try to find IWYU with version suffix
if(NOT IWYU_EXECUTABLE)
    find_program(IWYU_EXECUTABLE
        NAMES include-what-you-use-15 include-what-you-use-14 include-what-you-use-13
              include-what-you-use-12 include-what-you-use-11 include-what-you-use-10
        DOC "Include What You Use executable with version"
    )
endif()

if(IWYU_EXECUTABLE)
    set(IWYU_FOUND TRUE)
    message(STATUS "Found IWYU: ${IWYU_EXECUTABLE}")
    
    # Test if IWYU works with current compiler
    execute_process(
        COMMAND ${IWYU_EXECUTABLE} --version
        OUTPUT_VARIABLE IWYU_VERSION_OUTPUT
        ERROR_VARIABLE IWYU_VERSION_ERROR
        RESULT_VARIABLE IWYU_VERSION_RESULT
    )
    
    if(IWYU_VERSION_RESULT EQUAL 0)
        message(STATUS "IWYU version: ${IWYU_VERSION_OUTPUT}")
    else()
        message(WARNING "IWYU found but may not be compatible with current compiler")
    endif()
else()
    set(IWYU_FOUND FALSE)
    message(STATUS "IWYU not found - include analysis will be skipped")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(IWYU
    FOUND_VAR IWYU_FOUND
    REQUIRED_VARS IWYU_EXECUTABLE
)

mark_as_advanced(IWYU_EXECUTABLE)
