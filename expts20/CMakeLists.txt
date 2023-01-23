add_executable(demo hello.cc demo20.cc)
target_link_libraries(demo
    PRIVATE glog::glog
)
# CMake doesn't seem to support modules yet. This example can be built using:
#   g++ -std=c++20 -fmodules-ts hello.cc demo20.cc