cmake_minimum_required(VERSION 3.20)

project(stlx
    VERSION 1.0.0
    DESCRIPTION "STLX - C++23 utilities and examples"
    LANGUAGES CXX
)

# Set C++23 standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add cmake_modules to module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")

# Options for sanitizers and analysis tools
# NOTE: Only ONE sanitizer type can be enabled at a time
set(SANITIZER_TYPE "None" CACHE STRING "Sanitizer type: None, Address, UndefinedBehavior, Thread, Leak, Memory")
set_property(CACHE SANITIZER_TYPE PROPERTY STRINGS "None" "Address" "UndefinedBehavior" "Thread" "Leak" "Memory")

option(ENABLE_COVERAGE "Enable code coverage" OFF)
option(ENABLE_CLANG_TIDY "Enable clang-tidy static analysis" OFF)
option(ENABLE_CPPCHECK "Enable cppcheck static analysis" OFF)
option(ENABLE_CPPLINT "Enable cpplint style checking" OFF)
option(ENABLE_IWYU "Enable Include What You Use analysis" OFF)

# Find optional tools
find_package(CLANG_TIDY)
find_package(CLANG_FORMAT)
find_package(CPPCHECK)
find_package(VALGRIND)
find_package(GCOVR)
find_package(CPPLINT)
find_package(IWYU)

# Enable clang-tidy if found and requested
if(CLANG_TIDY_FOUND AND ENABLE_CLANG_TIDY)
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
endif()

# Configure FetchContent for GTest
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

# Create header-only library target
add_library(stlx INTERFACE)
target_include_directories(stlx INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Apply sanitizers to library if requested
if(NOT SANITIZER_TYPE STREQUAL "None")
    include(Sanitizers)
    apply_sanitizers(stlx)
endif()

# Enable testing
enable_testing()

# Add subdirectories
add_subdirectory(examples)
add_subdirectory(tests)

# Code coverage setup
if(ENABLE_COVERAGE)
    # Check if compiler supports coverage
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("-fprofile-arcs -ftest-coverage" COMPILER_SUPPORTS_COVERAGE)
    
    if(COMPILER_SUPPORTS_COVERAGE)
        message(STATUS "Enabling code coverage")
        
        # Add coverage flags to all targets
        add_compile_options(-fprofile-arcs -ftest-coverage)
        add_link_options(-fprofile-arcs -ftest-coverage)
        
        # Coverage targets
        if(GCOVR_FOUND)
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
        else()
            message(WARNING "gcovr not found - HTML coverage reports will not be available")
            
            add_custom_target(coverage_raw
                COMMAND ${CMAKE_COMMAND} -E echo "Raw coverage data generated in build directory"
                COMMENT "Raw coverage data (.gcda files) generated"
            )
            
            add_custom_target(coverage
                DEPENDS coverage_raw
                COMMENT "Generate raw coverage reports"
            )
        endif()
    else()
        message(WARNING "Compiler does not support coverage flags")
    endif()
endif()

# Static analysis targets with report generation
if(CPPCHECK_FOUND AND ENABLE_CPPCHECK)
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
endif()

# cpplint style checking with report generation
if(CPPLINT_FOUND AND ENABLE_CPPLINT)
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
endif()

# Include What You Use analysis with report generation
if(IWYU_FOUND AND ENABLE_IWYU)
    add_custom_target(iwyu
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/reports
        COMMAND ${IWYU_EXECUTABLE}
            -Xiwyu --no_fwd_decls
            ${CMAKE_CURRENT_SOURCE_DIR}/include/stlx/stlx.hpp
        COMMAND ${CMAKE_COMMAND} -E echo "IWYU analysis completed" > ${CMAKE_BINARY_DIR}/reports/iwyu.txt
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running Include What You Use analysis and generating report"
    )
endif()

# Code formatting target with report generation
if(CLANG_FORMAT_FOUND)
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
endif()

# Valgrind target for memory leak detection with report generation
if(VALGRIND_FOUND)
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
endif()

# Create reports directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/reports)

# Comprehensive report generation target
set(REPORT_TARGETS "")
if(CPPCHECK_FOUND AND ENABLE_CPPCHECK)
    list(APPEND REPORT_TARGETS cppcheck)
endif()
if(CPPLINT_FOUND AND ENABLE_CPPLINT)
    list(APPEND REPORT_TARGETS cpplint)
endif()
if(IWYU_FOUND AND ENABLE_IWYU)
    list(APPEND REPORT_TARGETS iwyu)
endif()
if(CLANG_FORMAT_FOUND)
    list(APPEND REPORT_TARGETS format)
endif()
if(CLANG_TIDY_FOUND AND ENABLE_CLANG_TIDY)
    list(APPEND REPORT_TARGETS clang-tidy-report)
endif()
if(VALGRIND_FOUND)
    list(APPEND REPORT_TARGETS valgrind-tests)
endif()

if(REPORT_TARGETS)
    add_custom_target(generate-all-reports
        DEPENDS ${REPORT_TARGETS}
        COMMENT "Generating all analysis reports"
    )
    
    # Create summary report
    add_custom_target(report-summary
        COMMAND ${CMAKE_COMMAND} -E echo "=== STLX Analysis Reports Summary ===" > ${CMAKE_BINARY_DIR}/reports/summary.txt
        COMMAND ${CMAKE_COMMAND} -E echo "Generated on: $$(date)" >> ${CMAKE_BINARY_DIR}/reports/summary.txt
        COMMAND ${CMAKE_COMMAND} -E echo "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}" >> ${CMAKE_BINARY_DIR}/reports/summary.txt
        COMMAND ${CMAKE_COMMAND} -E echo "C++ Standard: ${CMAKE_CXX_STANDARD}" >> ${CMAKE_BINARY_DIR}/reports/summary.txt
        COMMAND ${CMAKE_COMMAND} -E echo "" >> ${CMAKE_BINARY_DIR}/reports/summary.txt
        COMMAND ${CMAKE_COMMAND} -E echo "Available Reports:" >> ${CMAKE_BINARY_DIR}/reports/summary.txt
        COMMAND ${CMAKE_COMMAND} -E echo "- cppcheck.xml: Static analysis results" >> ${CMAKE_BINARY_DIR}/reports/summary.txt
        COMMAND ${CMAKE_COMMAND} -E echo "- cpplint.txt: Google C++ style checking" >> ${CMAKE_BINARY_DIR}/reports/summary.txt
        COMMAND ${CMAKE_COMMAND} -E echo "- iwyu.txt: Include What You Use analysis" >> ${CMAKE_BINARY_DIR}/reports/summary.txt
        COMMAND ${CMAKE_COMMAND} -E echo "- clang-format.txt: Code formatting results" >> ${CMAKE_BINARY_DIR}/reports/summary.txt
        COMMAND ${CMAKE_COMMAND} -E echo "- clang-tidy.txt: Clang static analysis" >> ${CMAKE_BINARY_DIR}/reports/summary.txt
        COMMAND ${CMAKE_COMMAND} -E echo "- clang-tidy-fixes.yaml: Suggested fixes" >> ${CMAKE_BINARY_DIR}/reports/summary.txt
        COMMAND ${CMAKE_COMMAND} -E echo "- valgrind.xml: Memory leak detection" >> ${CMAKE_BINARY_DIR}/reports/summary.txt
        DEPENDS generate-all-reports
        COMMENT "Generating analysis reports summary"
    )
endif()

# CPack configuration
include(CPack)
set(CPACK_PACKAGE_NAME "stlx")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "STLX - C++23 utilities and examples")
set(CPACK_PACKAGE_VENDOR "STLX Project")

# Create test_results component
cpack_add_component(test_results
    DISPLAY_NAME "Test Results and Analysis Reports"
    DESCRIPTION "Static and dynamic analysis reports, test results, and coverage data"
    REQUIRED FALSE
)

# Install reports to test_results component
if(REPORT_TARGETS)
    install(DIRECTORY ${CMAKE_BINARY_DIR}/reports/
        DESTINATION reports
        COMPONENT test_results
        FILES_MATCHING PATTERN "*.xml" PATTERN "*.txt" PATTERN "*.yaml"
    )
endif()

# Install test executables to test_results component
install(TARGETS stlx_tests
    RUNTIME DESTINATION bin
    COMPONENT test_results
)

# Print configuration summary
message(STATUS "")
message(STATUS "=== STLX Configuration Summary ===")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Sanitizer Type: ${SANITIZER_TYPE}")
message(STATUS "Code Coverage: ${ENABLE_COVERAGE}")
message(STATUS "clang-tidy: ${CLANG_TIDY_FOUND}")
message(STATUS "clang-format: ${CLANG_FORMAT_FOUND}")
message(STATUS "cppcheck: ${CPPCHECK_FOUND}")
message(STATUS "cpplint: ${CPPLINT_FOUND}")
message(STATUS "iwyu: ${IWYU_FOUND}")
message(STATUS "valgrind: ${VALGRIND_FOUND}")
message(STATUS "gcovr: ${GCOVR_FOUND}")
message(STATUS "Report Generation: ${REPORT_TARGETS}")
message(STATUS "==================================")
message(STATUS "")
