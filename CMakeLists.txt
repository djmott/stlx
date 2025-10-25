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
set(SANITIZER_TYPE "None" CACHE STRING "Sanitizer type: None, Address, UndefinedBehavior, Thread, Leak, Memory, All")
set_property(CACHE SANITIZER_TYPE PROPERTY STRINGS "None" "Address" "UndefinedBehavior" "Thread" "Leak" "Memory" "All")

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

# Static analysis targets
if(CPPCHECK_FOUND AND ENABLE_CPPCHECK)
    add_custom_target(cppcheck
        COMMAND ${CPPCHECK_EXECUTABLE} 
            --enable=all 
            --inconclusive 
            --std=c++23
            --suppress=missingIncludeSystem
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/examples
            ${CMAKE_CURRENT_SOURCE_DIR}/tests
        COMMENT "Running cppcheck static analysis"
    )
endif()

# cpplint style checking
if(CPPLINT_FOUND AND ENABLE_CPPLINT)
    file(GLOB_RECURSE CPPLINT_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp
    )
    add_custom_target(cpplint
        COMMAND ${CPPLINT_EXECUTABLE} 
            --linelength=100
            --filter=-build/include,-build/namespaces,-readability/namespace
            ${CPPLINT_SOURCES}
        COMMENT "Running cpplint style checking"
    )
endif()

# Include What You Use analysis
if(IWYU_FOUND AND ENABLE_IWYU)
    add_custom_target(iwyu
        COMMAND ${IWYU_EXECUTABLE}
            -Xiwyu --no_fwd_decls
            ${CMAKE_CURRENT_SOURCE_DIR}/include/stlx/stlx.hpp
        COMMENT "Running Include What You Use analysis"
    )
endif()

# Code formatting target
if(CLANG_FORMAT_FOUND)
    file(GLOB_RECURSE FORMAT_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp
    )
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXECUTABLE} -i ${FORMAT_SOURCES}
        COMMENT "Formatting code with clang-format"
    )
endif()

# Valgrind target for memory leak detection
if(VALGRIND_FOUND)
    add_custom_target(valgrind-tests
        COMMAND ${VALGRIND_EXECUTABLE} --leak-check=full --show-leak-kinds=all
            $<TARGET_FILE:stlx_tests>
        DEPENDS stlx_tests
        COMMENT "Running tests with valgrind memory leak detection"
    )
endif()

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
message(STATUS "==================================")
message(STATUS "")
