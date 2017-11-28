cmake_minimum_required(VERSION 3.9)

option(OPTION_IPLAYER_ENABLE_LOG "Verbose move" OFF)

set(IPLAYER_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

enable_testing()

add_subdirectory(src)
add_subdirectory(test)
