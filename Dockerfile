FROM ubuntu:22.04

# 安装构建依赖
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    mingw-w64 \
    && rm -rf /var/lib/apt/lists/*

# 设置工作目录
WORKDIR /opt/libyuv-build

# 复制libyuv源码
COPY libyuv-src /opt/libyuv-src

# 复制构建脚本
COPY build.sh /opt/build.sh
RUN chmod +x /opt/build.sh

# 复制测试程序和CMakeLists.txt
COPY test_digital_zoom.c /opt/test_digital_zoom.c
COPY CMakeLists.txt /opt/CMakeLists.txt
COPY run_test.sh /opt/run_test.sh
RUN chmod +x /opt/run_test.sh

# 创建输出目录
RUN mkdir -p /opt/output/x86 /opt/output/x64 /opt/output/test_results

# 设置默认命令
CMD ["/opt/build.sh"]