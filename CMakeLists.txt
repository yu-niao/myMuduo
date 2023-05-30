cmake_minimum_required(VERSION 3.26)
PROJECT(mymuduo)
set(CMAKE_CXX_STANDARD 17)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

aux_source_directory(. SRC_LIST)
#file(GLOB_SECURSE SRC_LIST .)

add_library(mymuduo SHARED ${SRC_LIST})