cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_STANDARD 20)
project(experiments VERSION 0.1.0)

include(CTest)
enable_testing()

# Configure top level cmake for deps, then build and install all deps
# The below calls can potentially be optimized
execute_process(COMMAND ${CMAKE_COMMAND} -S ${CMAKE_SOURCE_DIR}/deps -B ${CMAKE_BINARY_DIR}/deps -D CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
execute_process(COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/deps --config ${CMAKE_BUILD_TYPE})

# Append the install path of all deps
# The install paths need to match with the settings in deps/CMakeLists.txt
list(APPEND CMAKE_PREFIX_PATH
    ${CMAKE_BINARY_DIR}/deps/install/gflags
    ${CMAKE_BINARY_DIR}/deps/install/glog
    ${CMAKE_BINARY_DIR}/deps/install/GTest
    ${CMAKE_BINARY_DIR}/deps/install/benchmark
)
find_package(gflags REQUIRED NO_MODULE)
find_package(glog REQUIRED NO_MODULE)
find_package(GTest REQUIRED NO_MODULE)
find_package(benchmark REQUIRED NO_MODULE)

include(GoogleTest)

# Specify settings for clang-tidy
string(REGEX REPLACE "[\\\\|/]" "[\\\\/]" HEADERS "${CMAKE_BINARY_DIR}/../.*")

find_program(CLANG_TIDY clang-tidy)

if(CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY
        "${CLANG_TIDY}"
        "--header-filter=${HEADERS}"
        "--export-fixes=${CMAKE_BINARY_DIR}/clang-tidy.fixes")
endif()

add_subdirectory(expressions)
add_subdirectory(ky_expressions)
add_subdirectory(test_expressions)

add_subdirectory(iterators)
add_subdirectory(code_checks)
add_subdirectory(type_size_optimizations)
add_subdirectory(multi_reader_async)
add_subdirectory(cast_utils)
add_subdirectory(expressions2)
add_subdirectory(runtime_to_compiletime)

add_executable(example main.cc)
target_link_libraries(example
    PRIVATE glog::glog
    PRIVATE expressions)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)