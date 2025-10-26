# FindCLANG_FORMAT.cmake
# Find clang-format code formatting tool and create formatting targets

find_program(CLANG_FORMAT_EXECUTABLE
    NAMES clang-format
    DOC "clang-format executable"
)

if(CLANG_FORMAT_EXECUTABLE)
    set(CLANG_FORMAT_FOUND TRUE)
    message(STATUS "Found clang-format: ${CLANG_FORMAT_EXECUTABLE}")
    
    # Create format target
    file(GLOB_RECURSE FORMAT_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp
    )
    add_custom_target(format
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/reports
        COMMAND ${CLANG_FORMAT_EXECUTABLE} -i ${FORMAT_SOURCES}
        COMMAND ${CMAKE_COMMAND} -E echo "clang-format analysis completed" > ${CMAKE_BINARY_DIR}/reports/clang-format.txt
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Formatting code with clang-format and generating report"
    )
    
    # Export target for report generation
    if(NOT DEFINED _CLANG_FORMAT_REPORT_LIST)
        set(_CLANG_FORMAT_REPORT_LIST "format" PARENT_SCOPE)
    else()
        list(APPEND _CLANG_FORMAT_REPORT_LIST "format")
        set(_CLANG_FORMAT_REPORT_LIST ${_CLANG_FORMAT_REPORT_LIST} PARENT_SCOPE)
    endif()
else()
    set(CLANG_FORMAT_FOUND FALSE)
    message(STATUS "clang-format not found - code formatting will be skipped")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CLANG_FORMAT
    FOUND_VAR CLANG_FORMAT_FOUND
    REQUIRED_VARS CLANG_FORMAT_EXECUTABLE
)

mark_as_advanced(CLANG_FORMAT_EXECUTABLE)
