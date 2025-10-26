# FindIWYU.cmake
# Find Include What You Use (IWYU) tool and create analysis targets

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
    
    # Create IWYU target if requested
    if(ENABLE_IWYU)
        add_custom_target(iwyu
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/reports
            COMMAND ${IWYU_EXECUTABLE}
                -Xiwyu --no_fwd_decls
                ${CMAKE_CURRENT_SOURCE_DIR}/include/stlx/stlx.hpp
            COMMAND ${CMAKE_COMMAND} -E echo "IWYU analysis completed" > ${CMAKE_BINARY_DIR}/reports/iwyu.txt
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Running Include What You Use analysis and generating report"
        )
        
        # Export target for report generation
        if(NOT DEFINED _IWYU_REPORT_LIST)
            set(_IWYU_REPORT_LIST "iwyu" PARENT_SCOPE)
        else()
            list(APPEND _IWYU_REPORT_LIST "iwyu")
            set(_IWYU_REPORT_LIST ${_IWYU_REPORT_LIST} PARENT_SCOPE)
        endif()
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
