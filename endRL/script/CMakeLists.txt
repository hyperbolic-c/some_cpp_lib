aux_source_directory(. SRC_LIST)
include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)
# add_definitions(-std=c++11)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)  # C++ ABI question
add_executable(readOut ${SRC_LIST})
target_link_libraries(readOut libwavereader.a)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)