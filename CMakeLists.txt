cmake_minimum_required (VERSION 2.6)
project (nancy)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(vendor/twitcurl/libtwitcurl)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(Mysql REQUIRED)

add_subdirectory(vendor/yaml-cpp EXCLUDE_FROM_ALL)
include_directories(vendor/yaml-cpp/include)

include_directories(vendor/twitcurl/libtwitcurl ${MYSQL_INCLUDE_DIR})
add_executable(nancy nancy.cpp)
set_property(TARGET nancy PROPERTY CXX_STANDARD 11)
set_property(TARGET nancy PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(nancy yaml-cpp ${MYSQL_LIBRARIES} twitcurl curl)
