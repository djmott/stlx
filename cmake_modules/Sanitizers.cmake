# Sanitizers.cmake
# Function to apply sanitizer flags to targets

function(apply_sanitizers target_name)
    if(NOT SANITIZER_TYPE OR SANITIZER_TYPE STREQUAL "None")
        return()
    endif()

    # Check if compiler supports sanitizers
    include(CheckCXXCompilerFlag)
    
    set(SANITIZER_FLAGS "")
    
    if(SANITIZER_TYPE STREQUAL "Address" OR SANITIZER_TYPE STREQUAL "All")
        check_cxx_compiler_flag("-fsanitize=address" COMPILER_SUPPORTS_ADDRESS_SANITIZER)
        if(COMPILER_SUPPORTS_ADDRESS_SANITIZER)
            list(APPEND SANITIZER_FLAGS "-fsanitize=address")
            message(STATUS "Enabling Address Sanitizer for ${target_name}")
        endif()
    endif()
    
    if(SANITIZER_TYPE STREQUAL "UndefinedBehavior" OR SANITIZER_TYPE STREQUAL "All")
        check_cxx_compiler_flag("-fsanitize=undefined" COMPILER_SUPPORTS_UB_SANITIZER)
        if(COMPILER_SUPPORTS_UB_SANITIZER)
            list(APPEND SANITIZER_FLAGS "-fsanitize=undefined")
            message(STATUS "Enabling Undefined Behavior Sanitizer for ${target_name}")
        endif()
    endif()
    
    if(SANITIZER_TYPE STREQUAL "Thread" OR SANITIZER_TYPE STREQUAL "All")
        check_cxx_compiler_flag("-fsanitize=thread" COMPILER_SUPPORTS_THREAD_SANITIZER)
        if(COMPILER_SUPPORTS_THREAD_SANITIZER)
            list(APPEND SANITIZER_FLAGS "-fsanitize=thread")
            message(STATUS "Enabling Thread Sanitizer for ${target_name}")
        endif()
    endif()
    
    if(SANITIZER_TYPE STREQUAL "Leak" OR SANITIZER_TYPE STREQUAL "All")
        check_cxx_compiler_flag("-fsanitize=leak" COMPILER_SUPPORTS_LEAK_SANITIZER)
        if(COMPILER_SUPPORTS_LEAK_SANITIZER)
            list(APPEND SANITIZER_FLAGS "-fsanitize=leak")
            message(STATUS "Enabling Leak Sanitizer for ${target_name}")
        endif()
    endif()
    
    if(SANITIZER_TYPE STREQUAL "Memory" OR SANITIZER_TYPE STREQUAL "All")
        check_cxx_compiler_flag("-fsanitize=memory" COMPILER_SUPPORTS_MEMORY_SANITIZER)
        if(COMPILER_SUPPORTS_MEMORY_SANITIZER)
            list(APPEND SANITIZER_FLAGS "-fsanitize=memory")
            message(STATUS "Enabling Memory Sanitizer for ${target_name}")
        endif()
    endif()
    
    if(SANITIZER_FLAGS)
        # Only apply sanitizers to non-INTERFACE targets
        get_target_property(target_type ${target_name} TYPE)
        if(NOT target_type STREQUAL "INTERFACE_LIBRARY")
            target_compile_options(${target_name} PRIVATE ${SANITIZER_FLAGS})
            target_link_options(${target_name} PRIVATE ${SANITIZER_FLAGS})
        endif()
        
        # Add runtime environment variables for sanitizers
        if(SANITIZER_TYPE STREQUAL "Address" OR SANITIZER_TYPE STREQUAL "All")
            set_property(TARGET ${target_name} PROPERTY ENVIRONMENT "ASAN_OPTIONS=abort_on_error=1")
        endif()
        
        if(SANITIZER_TYPE STREQUAL "UndefinedBehavior" OR SANITIZER_TYPE STREQUAL "All")
            set_property(TARGET ${target_name} PROPERTY ENVIRONMENT "UBSAN_OPTIONS=abort_on_error=1")
        endif()
    endif()
endfunction()
