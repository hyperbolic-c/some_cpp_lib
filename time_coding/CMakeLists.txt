cmake_minimum_required(VERSION 2.8)
project(timeCoding)
# add_definitions(-std=c++11)
aux_source_directory(. SRC_LIST)
add_executable(timeCoding ${SRC_LIST})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)