cmake_minimum_required (VERSION 2.6)
project (nancy)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(vendor/twitcurl/libtwitcurl)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(PkgConfig)
pkg_check_modules(YamlCpp yaml-cpp REQUIRED)
find_package(Mysql REQUIRED)

include_directories(vendor/twitcurl/libtwitcurl ${MYSQL_INCLUDE_DIR})
add_executable(nancy nancy.cpp)
set_property(TARGET nancy PROPERTY CXX_STANDARD 11)
set_property(TARGET nancy PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(nancy ${YamlCpp_LIBRARIES} ${MYSQL_LIBRARIES} twitcurl curl)
