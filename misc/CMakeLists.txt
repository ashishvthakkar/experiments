add_executable(max_products max_products.cc)
target_link_libraries(max_products
    PRIVATE glog::glog
)

add_executable(payoffs payoffs.cc)
target_link_libraries(payoffs
    PRIVATE glog::glog
)
