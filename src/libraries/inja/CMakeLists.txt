cmake_minimum_required(VERSION 3.14)

add_library(inja INTERFACE inja.hpp json.hpp)

target_include_directories(inja INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)

install(FILES inja.hpp json.hpp DESTINATION include)
