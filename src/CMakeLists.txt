# 会查找当前目录（.）下的所有源文件，并将这些文件的列表存储在变量SRC_LIST中
aux_source_directory(. SRC_LIST)

# 查找当前protobuf目录下的所有文件，并将这些文件的列表存储在变量PROTOBUF_SRC_LIST中
aux_source_directory(./protobuf PROTOBUF_SRC_LIST)

# 生成动态库
add_library(mprpc SHARED ${SRC_LIST} ${PROTOBUF_SRC_LIST})

# 设置动态库的版本号
set_target_properties(mprpc PROPERTIES VERSION 0.1 SOVERSION 1)

# 链接动态库
target_link_libraries(mprpc muduo_net muduo_base pthread)