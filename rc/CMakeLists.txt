add_executable(simple_monitor simple_monitor.cc)
target_link_libraries(simple_monitor
    PRIVATE glog::glog
)

add_executable(simple_spsc spsc_q.cc)
target_link_libraries(simple_spsc
    PRIVATE glog::glog
)

add_executable(simple_spsc_tests spsc_q.cc spsc_q_tests.cc)
target_link_libraries(simple_spsc_tests
    PRIVATE glog::glog
    PRIVATE GTest::gtest
    PRIVATE GTest::gtest_main
    )
gtest_discover_tests(simple_spsc_tests)
