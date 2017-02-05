cmake_minimum_required(VERSION 3.5)
project(cpp_dep VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
add_library(cpp_dep src/cpp_dep.cpp)
target_include_directories(cpp_dep PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src)

find_package(Boost REQUIRED)
target_link_libraries(cpp_dep Boost::boost)
