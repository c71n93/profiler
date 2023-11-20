cmake_minimum_required(VERSION 3.22)
project(profiler LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)

find_package(LLVM CONFIG)

if (NOT DEFINED SOURCE)
    message(STATUS "Path wasn't specified, using default")
    set(SOURCE resources/fact.c)
endif()

if(LLVM_FOUND)
    add_library(profiler SHARED src/prof-pass.cpp)
    target_include_directories(profiler PUBLIC ${LLVM_INCLUDE_DIRS})
    target_link_libraries(profiler PRIVATE LLVM)
else()
    message(FATAL_ERROR "LLVM not found")
endif()

set(LOGGER_SOURCE_PATH ${CMAKE_SOURCE_DIR}/src/prof-functions/log.cpp)

function(run_profiler_single_source FILE_NAME)
    add_custom_target(run_profiler
            COMMAND clang -O2 -Xclang -load -Xclang $<TARGET_FILE_DIR:profiler>/$<TARGET_FILE_NAME:profiler> ${FILE_NAME} ${LOGGER_SOURCE_PATH} -o run_profiler
            COMMENT "Instrumenting ${FILE_NAME}"
    )
    add_dependencies(run_profiler profiler)
endfunction()

#function(profile_single_source FILE_NAME)
#    set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/out)
#    set(OUT_EXECUTABLE ${OUTPUT_DIR}/instrumented)
#    set(OUT_LOG ${OUTPUT_DIR}/profile.log)
#    add_custom_command(
#            OUTPUT ${OUTPUT_DIR}
#            COMMAND mkdir -p ${OUTPUT_DIR}
#            COMMENT "Creating output directory"
#    )
#    add_custom_command(
#            OUTPUT ${OUT_EXECUTABLE}
#            COMMAND clang -O2 -Xclang -load -Xclang $<TARGET_FILE_DIR:profiler>/$<TARGET_FILE_NAME:profiler> ${FILE_NAME} ${LOGGER_SOURCE_PATH} -o ${OUT_EXECUTABLE}
#            COMMENT "Instrumenting ${FILE_NAME}"
#    )
#
#    add_custom_command(
#            OUTPUT ${OUT_LOG}
#            DEPENDS ${OUT_EXECUTABLE}
#            COMMAND ./${OUT_EXECUTABLE} 5 > ${OUT_LOG}
#            COMMENT "Instrumenting ${FILE_NAME}"
#    )
#endfunction()

function(obtain_instrumented_object IN_SOURCE_PATH OUT_OBJECT_PATH)
    add_custom_command(
            OUTPUT ${OUT_OBJECT_PATH}
            COMMAND clang -O2 -Xclang -load -Xclang $<TARGET_FILE_DIR:profiler>/$<TARGET_FILE_NAME:profiler> ${IN_SOURCE_PATH} ${LOGGER_SOURCE_PATH} -c ${OUT_OBJECT_PATH}
            COMMENT "Instrumenting ${IN_SOURCE_PATH}"
    )
endfunction()

function(profile_instrumented_executable EXECUTABLE)
    message(STATUS "not implemented yet")
endfunction()

run_profiler_single_source(${SOURCE})
