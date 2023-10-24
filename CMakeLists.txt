cmake_minimum_required(VERSION 3.22)
project(profiler LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)

find_package(LLVM CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
if(LLVM_FOUND)
    add_library(profiler SHARED src/prof-pass.cpp)
    target_include_directories(profiler PUBLIC ${LLVM_INCLUDE_DIRS})
    target_sources(profiler PRIVATE
            src/prof-functions/log.cpp
    )
    target_link_libraries(profiler PRIVATE LLVM)

    add_custom_target(run_profiler
            COMMENT "Instrumenting ${SOURCE}"
            COMMAND clang -Xclang -load -Xclang $<TARGET_FILE_DIR:profiler>/$<TARGET_FILE_NAME:profiler> ${SOURCE} ${CMAKE_SOURCE_DIR}/src/prof-functions/log.cpp -o run_profiler
    )
endif()
