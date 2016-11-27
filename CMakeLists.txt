cmake_minimum_required (VERSION 3.1)
project (nancy)

add_subdirectory(vendor/libtwittercpp)
add_subdirectory(vendor/verbly)
add_subdirectory(vendor/yaml-cpp EXCLUDE_FROM_ALL)

include_directories(vendor/yaml-cpp/include vendor/libtwittercpp/src vendor/verbly/lib)
add_executable(nancy nancy.cpp)
set_property(TARGET nancy PROPERTY CXX_STANDARD 11)
set_property(TARGET nancy PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(nancy yaml-cpp twitter++ verbly)
