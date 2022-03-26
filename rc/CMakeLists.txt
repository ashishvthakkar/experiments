add_executable(simple_monitor simple_monitor.cc)
target_link_libraries(simple_monitor
    PRIVATE glog::glog
)
