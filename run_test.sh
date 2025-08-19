#!/bin/bash

# 构建测试程序
echo "构建测试程序..."
cd /opt
mkdir -p build_test
cd build_test

# 使用系统默认编译器构建测试程序（Linux版本）
cmake /opt
make -j$(nproc)

echo "运行测试程序..."
echo "注意: 这将处理3600帧(1920x1080分辨率)，可能需要几分钟时间完成..."
echo "处理过程中将显示每帧的执行时间..."
./test_digital_zoom

# 将生成的YUV文件复制到输出目录
echo "将结果复制到输出目录..."
cp zoomed.yuv /opt/output/ 2>/dev/null || echo "无法复制YUV文件到输出目录"
echo "测试结果已保存到 /opt/output/ 目录"