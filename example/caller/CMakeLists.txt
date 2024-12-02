# 自定义变量，指定所有源文件
aux_source_directory(. SRC_LIST)
aux_source_directory(.. LAST_SRC_LIST)

# 指定生成可执行文件
add_executable(consumer ${SRC_LIST} ${LAST_SRC_LIST})

# 指定链接库
target_link_libraries(consumer protobuf mprpc)