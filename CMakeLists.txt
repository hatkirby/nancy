cmake_minimum_required (VERSION 2.6)
project (nancy)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(vendor/twitcurl/libtwitcurl)
add_subdirectory(vendor/verbly)
add_subdirectory(vendor/yaml-cpp EXCLUDE_FROM_ALL)

find_package(PkgConfig)
pkg_check_modules(sqlite3 sqlite3 REQUIRED)

include_directories(vendor/yaml-cpp/include vendor/twitcurl/libtwitcurl vendor/verbly/lib)
add_executable(nancy nancy.cpp)
set_property(TARGET nancy PROPERTY CXX_STANDARD 11)
set_property(TARGET nancy PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(nancy yaml-cpp twitcurl curl verbly ${sqlite3_LIBRARIES})
