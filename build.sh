#!/bin/bash

set -e

echo "开始编译libyuv for Windows"

# 创建输出目录
mkdir -p /opt/output/x86
mkdir -p /opt/output/x64

# 编译x86版本
echo "编译x86版本..."
mkdir -p /opt/build/x86
cd /opt/build/x86

# 使用标准的cmake命令和工具链文件
cmake -DCMAKE_TOOLCHAIN_FILE=/opt/libyuv-src/cmake/toolchain-mingw-x86.cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/opt/install/x86 \
  /opt/libyuv-src

make -j$(nproc)
make install

# 复制结果到输出目录
cp /opt/install/x86/lib/*.a /opt/output/x86/
cp /opt/install/x86/bin/*.dll /opt/output/x86/ 2>/dev/null || echo "No DLL files to copy for x86"
cp -r /opt/install/x86/include /opt/output/x86/

# 编译x64版本
echo "编译x64版本..."
mkdir -p /opt/build/x64
cd /opt/build/x64

cmake -DCMAKE_TOOLCHAIN_FILE=/opt/libyuv-src/cmake/toolchain-mingw-x64.cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/opt/install/x64 \
  /opt/libyuv-src

make -j$(nproc)
make install

# 复制结果到输出目录
cp /opt/install/x64/lib/*.a /opt/output/x64/
cp /opt/install/x64/bin/*.dll /opt/output/x64/ 2>/dev/null || echo "No DLL files to copy for x64"
cp -r /opt/install/x64/include /opt/output/x64/

# 构建测试程序 (Linux版本用于验证)
# echo "构建测试程序..."
# cd /opt
# mkdir -p /opt/build_test
# cd /opt/build_test

# # 使用系统默认编译器构建测试程序
# cmake /opt
# make -j$(nproc)

echo "编译完成!"
echo "x86版本位于: /opt/output/x86/"
echo "x64版本位于: /opt/output/x64/"
echo "测试程序位于: /opt/build_test/test_digital_zoom"