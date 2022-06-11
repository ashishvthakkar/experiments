add_library(max_products_lib max_products.cc)
target_link_libraries(max_products_lib
    PRIVATE glog::glog)

add_executable(max_products max_products_demo.cc)
target_link_libraries(max_products
    PRIVATE max_products_lib
    PRIVATE glog::glog)

add_executable(payoffs payoffs.cc)
target_link_libraries(payoffs
    PRIVATE glog::glog)

add_executable(max_products_tests max_products_tests.cc)
target_link_libraries(max_products_tests
    PRIVATE max_products_lib
    PRIVATE glog::glog
    PRIVATE GTest::gtest
    PRIVATE GTest::gtest_main)
gtest_discover_tests(max_products_tests)
