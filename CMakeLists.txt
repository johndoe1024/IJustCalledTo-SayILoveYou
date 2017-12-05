cmake_minimum_required(VERSION 3.9)

option(OPTION_IPLAYER_ENABLE_LOG "Verbose move" OFF)
option(OPTION_IPLAYER_DECODER_MAD "Enable libmad decoder" OFF)

set(IPLAYER_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

enable_testing()

add_subdirectory(src)
add_subdirectory(test)
