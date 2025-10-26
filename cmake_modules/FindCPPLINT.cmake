# FindCPPLINT.cmake
# Find cpplint Python script for Google C++ style checking and create analysis targets

find_program(CPPLINT_EXECUTABLE
    NAMES cpplint
    DOC "cpplint executable"
)

# Also try to find cpplint.py script
if(NOT CPPLINT_EXECUTABLE)
    find_file(CPPLINT_SCRIPT
        NAMES cpplint.py
        PATHS /usr/local/bin /usr/bin /opt/local/bin
        DOC "cpplint.py script"
    )
    if(CPPLINT_SCRIPT)
        set(CPPLINT_EXECUTABLE ${CPPLINT_SCRIPT})
    endif()
endif()

# Try to find cpplint via Python module
if(NOT CPPLINT_EXECUTABLE)
    find_program(PYTHON_EXECUTABLE
        NAMES python3 python
        DOC "Python executable"
    )
    if(PYTHON_EXECUTABLE)
        execute_process(
            COMMAND ${PYTHON_EXECUTABLE} -c "import cpplint; print(cpplint.__file__)"
            OUTPUT_VARIABLE CPPLINT_MODULE_PATH
            ERROR_QUIET
            RESULT_VARIABLE CPPLINT_MODULE_RESULT
        )
        if(CPPLINT_MODULE_RESULT EQUAL 0)
            get_filename_component(CPPLINT_MODULE_DIR ${CPPLINT_MODULE_PATH} DIRECTORY)
            find_file(CPPLINT_SCRIPT
                NAMES cpplint.py
                PATHS ${CPPLINT_MODULE_DIR}
                NO_DEFAULT_PATH
                DOC "cpplint.py from Python module"
            )
            if(CPPLINT_SCRIPT)
                set(CPPLINT_EXECUTABLE ${PYTHON_EXECUTABLE} ${CPPLINT_SCRIPT})
            endif()
        endif()
    endif()
endif()

if(CPPLINT_EXECUTABLE)
    set(CPPLINT_FOUND TRUE)
    message(STATUS "Found cpplint: ${CPPLINT_EXECUTABLE}")
    
    # Create cpplint target if requested
    if(ENABLE_CPPLINT)
        file(GLOB_RECURSE CPPLINT_SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/examples/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp
        )
        add_custom_target(cpplint
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/reports
            COMMAND ${CPPLINT_EXECUTABLE} 
                --linelength=100
                --filter=-build/include,-build/namespaces,-readability/namespace
                --output=vs7
                ${CPPLINT_SOURCES}
            COMMAND ${CMAKE_COMMAND} -E echo "cpplint analysis completed" > ${CMAKE_BINARY_DIR}/reports/cpplint.txt
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Running cpplint style checking and generating report"
        )
        
        # Export target for report generation
        if(NOT DEFINED _CPPLINT_REPORT_LIST)
            set(_CPPLINT_REPORT_LIST "cpplint" PARENT_SCOPE)
        else()
            list(APPEND _CPPLINT_REPORT_LIST "cpplint")
            set(_CPPLINT_REPORT_LIST ${_CPPLINT_REPORT_LIST} PARENT_SCOPE)
        endif()
    endif()
else()
    set(CPPLINT_FOUND FALSE)
    message(STATUS "cpplint not found - Google C++ style checking will be skipped")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CPPLINT
    FOUND_VAR CPPLINT_FOUND
    REQUIRED_VARS CPPLINT_EXECUTABLE
)

mark_as_advanced(CPPLINT_EXECUTABLE CPPLINT_SCRIPT)
