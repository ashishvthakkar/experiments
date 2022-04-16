add_executable(simple_monitor simple_monitor.cc)
target_link_libraries(simple_monitor
    PRIVATE glog::glog
)

add_executable(simple_spsc spsc_q.cc)
target_link_libraries(simple_spsc
    PRIVATE glog::glog
)
