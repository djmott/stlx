# Examples CMakeLists.txt
# Build both simple and complex example applications

# Simple hello world example
add_executable(hello_world simple/hello_world.cpp)
target_link_libraries(hello_world stlx)

# Apply sanitizers if requested
if(NOT SANITIZER_TYPE STREQUAL "None")
    include(Sanitizers)
    apply_sanitizers(hello_world)
endif()

# Complex multi-file example
add_executable(complex_example 
    complex/main.cpp
    complex/module_a.cpp
)
target_link_libraries(complex_example stlx)

# Apply sanitizers if requested
if(NOT SANITIZER_TYPE STREQUAL "None")
    apply_sanitizers(complex_example)
endif()

# Parser examples
add_executable(parser_examples parser_examples.cpp)
target_link_libraries(parser_examples stlx)

# Apply sanitizers if requested
if(NOT SANITIZER_TYPE STREQUAL "None")
    apply_sanitizers(parser_examples)
endif()

# Parse tree demo example
add_executable(parse_tree_demo parse_tree_demo.cpp)
target_link_libraries(parse_tree_demo stlx)

# Apply sanitizers if requested
if(NOT SANITIZER_TYPE STREQUAL "None")
    apply_sanitizers(parse_tree_demo)
endif()

# BASIC grammar parse tree example
add_executable(abasic_parse_tree_example abasic_parse_tree_example.cpp)
target_link_libraries(abasic_parse_tree_example stlx)

# Apply sanitizers if requested
if(NOT SANITIZER_TYPE STREQUAL "None")
    apply_sanitizers(abasic_parse_tree_example)
endif()

# Command line parser example (disabled - needs refactoring for STLX parser design)
# add_executable(parser1 complex/parser1.cpp)
# target_link_libraries(parser1 stlx)

# Apply sanitizers if requested
# if(NOT SANITIZER_TYPE STREQUAL "None")
#     apply_sanitizers(parser1)
# endif()

# Set output directories
set_target_properties(hello_world complex_example parser_examples parse_tree_demo abasic_parse_tree_example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)