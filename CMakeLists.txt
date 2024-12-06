cmake_minimum_required(VERSION 3.28.3)
project(MPRPC)

set(CMAKE_CXX_STANDARD 20)

# 设置项目可执行文件输出的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 设置项目库文件输出的路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 设置头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/example)

# 设置库文件搜索路径
link_directories(${PROJECT_SOURCE_DIR}/lib)

# 包含子目录
add_subdirectory(src)
add_subdirectory(example)