#!/bin/bash

# 在脚本执行过程中遇到错误时立即退出
set -e

# 删除当前目录下的build文件夹及其内容
rm -rf `pwd`/build/*

# 创建build文件夹
mkdir -p `pwd`/build

# 进入build文件夹
cd `pwd`/build &&
    # 使用cmake命令生成Makefile
    cmake ..&&
    # 使用make命令编译
    make

# 返回上一级目录
cd ..

# 将src文件夹下的include文件夹复制到lib文件夹下
cp -r `pwd`/src/include `pwd`/lib