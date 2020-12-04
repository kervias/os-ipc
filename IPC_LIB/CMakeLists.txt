cmake_minimum_required(VERSION 3.0)
project(SOCKET_TCP C)

set(CMAKE_C_STANDARD 99)

# 设置lib文件输出路径
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/lib)
# make install 命令生成的文件目录
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

# 将所有IPC生成一个包
file(GLOB_RECURSE DIR_LIB_SRCS ${PROJECT_SOURCE_DIR}/src/*.c)
add_library (IPC_STATIC ${DIR_LIB_SRCS})
set_target_properties(IPC_STATIC PROPERTIES OUTPUT_NAME IPC)

# 添加到动态链接库中
add_library (IPC SHARED ${DIR_LIB_SRCS})
# 指定动态链接库不带后缀的名字
set_target_properties(IPC PROPERTIES OUTPUT_NAME IPC)
