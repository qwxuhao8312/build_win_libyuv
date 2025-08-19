cmake_minimum_required(VERSION 3.10)
project(test_digital_zoom)

# 设置C标准
set(CMAKE_C_STANDARD 99)

# 查找libyuv库
find_library(LIBYUV_LIBRARY
    NAMES yuv libyuv
    PATHS /usr/local/lib /usr/lib ${CMAKE_CURRENT_SOURCE_DIR}/output/x64
)

# 查找libyuv头文件
find_path(LIBYUV_INCLUDE_DIR
    NAMES libyuv.h
    PATHS /usr/local/include ${CMAKE_CURRENT_SOURCE_DIR}/output/x64/include
)

# 如果在当前项目中找到了libyuv源码，则编译它
if(NOT LIBYUV_LIBRARY OR NOT LIBYUV_INCLUDE_DIR)
    # 尝试使用本地libyuv源码
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libyuv-src")
        message(STATUS "使用本地libyuv源码")
        add_subdirectory(libyuv-src)
        set(LIBYUV_LIBRARY yuv)
        set(LIBYUV_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libyuv-src/include")
    else()
        message(FATAL_ERROR "未找到libyuv库")
    endif()
endif()

# 创建可执行文件
add_executable(test_digital_zoom test_digital_zoom.c)

# 链接libyuv库
target_link_libraries(test_digital_zoom ${LIBYUV_LIBRARY})

# 包含libyuv头文件
target_include_directories(test_digital_zoom PRIVATE ${LIBYUV_INCLUDE_DIR})