# FindVALGRIND.cmake
# Find valgrind dynamic analysis tool and create analysis targets

find_program(VALGRIND_EXECUTABLE
    NAMES valgrind
    DOC "valgrind executable"
)

if(VALGRIND_EXECUTABLE)
    set(VALGRIND_FOUND TRUE)
    message(STATUS "Found valgrind: ${VALGRIND_EXECUTABLE}")
    
    # Create valgrind test target
    # Note: This assumes stlx_tests target exists
    # The target will be created if stlx_tests is available
    add_custom_target(valgrind-tests
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/reports
        COMMAND ${VALGRIND_EXECUTABLE} 
            --leak-check=full 
            --show-leak-kinds=all
            --xml=yes
            --xml-file=${CMAKE_BINARY_DIR}/reports/valgrind.xml
            $<TARGET_FILE:stlx_tests>
        DEPENDS stlx_tests
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running tests with valgrind memory leak detection and generating XML report"
    )
    
    # Export target for report generation
    if(NOT DEFINED _VALGRIND_REPORT_LIST)
        set(_VALGRIND_REPORT_LIST "valgrind-tests" PARENT_SCOPE)
    else()
        list(APPEND _VALGRIND_REPORT_LIST "valgrind-tests")
        set(_VALGRIND_REPORT_LIST ${_VALGRIND_REPORT_LIST} PARENT_SCOPE)
    endif()
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
