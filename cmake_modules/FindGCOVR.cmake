# FindGCOVR.cmake
# Find gcovr code coverage tool and create coverage targets

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

# Create coverage targets if requested
if(ENABLE_COVERAGE AND GCOVR_FOUND)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("-fprofile-arcs -ftest-coverage" COMPILER_SUPPORTS_COVERAGE)
    
    if(COMPILER_SUPPORTS_COVERAGE)
        message(STATUS "Enabling code coverage")
        add_compile_options(-fprofile-arcs -ftest-coverage)
        add_link_options(-fprofile-arcs -ftest-coverage)
        
        add_custom_target(coverage_raw
            COMMAND ${CMAKE_COMMAND} -E echo "Raw coverage data generated in build directory"
            COMMENT "Raw coverage data (.gcda files) generated"
        )
        add_custom_target(coverage_html
            COMMAND ${GCOVR_EXECUTABLE} --html --html-details -o coverage_report.html
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating HTML coverage report"
        )
        add_custom_target(coverage
            DEPENDS coverage_raw coverage_html
            COMMENT "Generate both raw and HTML coverage reports"
        )
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GCOVR
    FOUND_VAR GCOVR_FOUND
    REQUIRED_VARS GCOVR_EXECUTABLE
)

mark_as_advanced(GCOVR_EXECUTABLE)
