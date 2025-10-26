# FindCLANG_TIDY.cmake
# Find clang-tidy static analysis tool and create analysis targets

find_program(CLANG_TIDY_EXECUTABLE
    NAMES clang-tidy
    DOC "clang-tidy executable"
)

if(CLANG_TIDY_EXECUTABLE)
    set(CLANG_TIDY_FOUND TRUE)
    message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXECUTABLE}")
    
    # Enable clang-tidy if requested
    if(ENABLE_CLANG_TIDY)
        set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXECUTABLE})
        message(STATUS "Enabled clang-tidy static analysis")
        
        # Create clang-tidy report target
        file(GLOB_RECURSE CLANG_TIDY_SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/examples/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp
        )
        add_custom_target(clang-tidy-report
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/reports
            COMMAND ${CLANG_TIDY_EXECUTABLE}
                --format-style=file
                --export-fixes=${CMAKE_BINARY_DIR}/reports/clang-tidy-fixes.yaml
                ${CLANG_TIDY_SOURCES}
            COMMAND ${CMAKE_COMMAND} -E echo "clang-tidy analysis completed" > ${CMAKE_BINARY_DIR}/reports/clang-tidy.txt
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Running clang-tidy static analysis and generating report"
        )
        
        # Export target for report generation
        if(NOT DEFINED _CLANG_TIDY_REPORT_LIST)
            set(_CLANG_TIDY_REPORT_LIST "clang-tidy-report" PARENT_SCOPE)
        else()
            list(APPEND _CLANG_TIDY_REPORT_LIST "clang-tidy-report")
            set(_CLANG_TIDY_REPORT_LIST ${_CLANG_TIDY_REPORT_LIST} PARENT_SCOPE)
        endif()
    endif()
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
