# FindCPPCHECK.cmake
# Find cppcheck static analysis tool and create analysis targets

find_program(CPPCHECK_EXECUTABLE
    NAMES cppcheck
    DOC "cppcheck executable"
)

if(CPPCHECK_EXECUTABLE)
    set(CPPCHECK_FOUND TRUE)
    message(STATUS "Found cppcheck: ${CPPCHECK_EXECUTABLE}")
    
    # Create cppcheck target if requested
    if(ENABLE_CPPCHECK)
        add_custom_target(cppcheck
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/reports
            COMMAND ${CPPCHECK_EXECUTABLE} 
                --enable=all 
                --inconclusive 
                --std=c++23
                --suppress=missingIncludeSystem
                --xml
                --xml-version=2
                ${CMAKE_CURRENT_SOURCE_DIR}/include
                ${CMAKE_CURRENT_SOURCE_DIR}/examples
                ${CMAKE_CURRENT_SOURCE_DIR}/tests
                > ${CMAKE_BINARY_DIR}/reports/cppcheck.xml
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Running cppcheck static analysis and generating XML report"
        )
        
        # Export target for report generation
        if(NOT DEFINED _CPPCHECK_REPORT_LIST)
            set(_CPPCHECK_REPORT_LIST "cppcheck" PARENT_SCOPE)
        else()
            list(APPEND _CPPCHECK_REPORT_LIST "cppcheck")
            set(_CPPCHECK_REPORT_LIST ${_CPPCHECK_REPORT_LIST} PARENT_SCOPE)
        endif()
    endif()
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
