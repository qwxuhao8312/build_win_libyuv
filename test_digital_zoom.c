#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>  // For timing
#include "libyuv.h"

#define WIDTH 1920
#define HEIGHT 1080
#define FPS 60
#define DURATION 10  // 10s
#define TOTAL_FRAMES (FPS * DURATION)
#define START_SCALE 1.0
#define END_SCALE 8.0

// 生成一个带有彩色方块的测试图像
void generate_test_image(uint8_t* y_plane, uint8_t* u_plane, uint8_t* v_plane) {
    int x, y;
    
    // 填充黑色背景
    memset(y_plane, 0, WIDTH * HEIGHT);  // Y = 0 (黑色)
    memset(u_plane, 128, WIDTH * HEIGHT / 2);  // U = 128 (中性色度)
    memset(v_plane, 128, WIDTH * HEIGHT / 2);  // V = 128 (中性色度)
    
    // 在图像中心绘制一个红色方块 (200x200)
    int square_size = 200;
    int start_x = (WIDTH - square_size) / 2;
    int start_y = (HEIGHT - square_size) / 2;
    
    for (y = start_y; y < start_y + square_size; y++) {
        for (x = start_x; x < start_x + square_size; x++) {
            // 红色在YUV空间中的近似值
            y_plane[y * WIDTH + x] = 76;   // Y
            // 注意U/V平面是每两个水平像素共享一个值
            if (x % 2 == 0) {
                u_plane[y * WIDTH/2 + x/2] = 85;   // U
                v_plane[y * WIDTH/2 + x/2] = 255;  // V
            }
        }
    }
    
    // 在红色方块内部绘制一个绿色方块 (100x100)
    int inner_square_size = 100;
    int inner_start_x = (WIDTH - inner_square_size) / 2;
    int inner_start_y = (HEIGHT - inner_square_size) / 2;
    
    for (y = inner_start_y; y < inner_start_y + inner_square_size; y++) {
        for (x = inner_start_x; x < inner_start_x + inner_square_size; x++) {
            // 绿色在YUV空间中的近似值
            y_plane[y * WIDTH + x] = 150;  // Y
            if (x % 2 == 0) {
                u_plane[y * WIDTH/2 + x/2] = 44;   // U
                v_plane[y * WIDTH/2 + x/2] = 21;   // V
            }
        }
    }
}

// 保存YUV数据到文件
void SaveYUVToFile(const char* filename, uint8_t* y, uint8_t* u, uint8_t* v, int width, int height) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("无法打开文件 %s 进行写入\n", filename);
        return;
    }
    
    // 写入Y分量
    fwrite(y, 1, width * height, file);
    
    // 写入U分量
    fwrite(u, 1, width * height / 2, file);
    
    // 写入V分量
    fwrite(v, 1, width * height / 2, file);
    
    fclose(file);
    printf("图像已保存到 %s\n", filename);
}

// 执行数码变焦处理
int process_zoom_frame(
    uint8_t* original_y, uint8_t* original_u, uint8_t* original_v,
    uint8_t* output_y, uint8_t* output_u, uint8_t* output_v,
    float scale_factor) {
    
    // 计算裁剪区域
    int crop_width = (int)(WIDTH / scale_factor);
    int crop_height = (int)(HEIGHT / scale_factor);
    
    // 确保裁剪区域不小于1像素
    if (crop_width < 1) crop_width = 1;
    if (crop_height < 1) crop_height = 1;
    
    int crop_x = (WIDTH - crop_width) / 2;
    int crop_y = (HEIGHT - crop_height) / 2;
    
    // 分配裁剪后图像的内存
    uint8_t* cropped_y = (uint8_t*)malloc(crop_width * crop_height);
    uint8_t* cropped_u = (uint8_t*)malloc(crop_width * crop_height / 2);
    uint8_t* cropped_v = (uint8_t*)malloc(crop_width * crop_height / 2);
    
    if (!cropped_y || !cropped_u || !cropped_v) {
        printf("裁剪图像内存分配失败\n");
        free(cropped_y);
        free(cropped_u);
        free(cropped_v);
        return -1;
    }
    
    // 手动实现裁剪功能 (Y平面)
    for (int y = 0; y < crop_height; y++) {
        memcpy(cropped_y + y * crop_width, 
               original_y + (crop_y + y) * WIDTH + crop_x, 
               crop_width);
    }
    
    // 手动实现裁剪功能 (U/V平面)
    for (int y = 0; y < crop_height; y++) {
        memcpy(cropped_u + y * crop_width/2, 
               original_u + (crop_y + y) * WIDTH/2 + crop_x/2, 
               crop_width/2);
        memcpy(cropped_v + y * crop_width/2, 
               original_v + (crop_y + y) * WIDTH/2 + crop_x/2, 
               crop_width/2);
    }
    
    // 使用libyuv的I422Scale函数将裁剪后的图像放大到原始尺寸
    int result = I422Scale(
        cropped_y, crop_width,
        cropped_u, crop_width/2,
        cropped_v, crop_width/2,
        crop_width, crop_height,
        output_y, WIDTH,
        output_u, WIDTH/2,
        output_v, WIDTH/2,
        WIDTH, HEIGHT,
        kFilterBilinear
    );
    
    // 释放裁剪图像内存
    free(cropped_y);
    free(cropped_u);
    free(cropped_v);
    
    return result;
}

int main() {
    printf("开始生成测试数据和执行数码变焦处理...\n");
    printf("分辨率: %dx%d, 帧率: %d fps, 时长: %d 秒\n", WIDTH, HEIGHT, FPS, DURATION);
    printf("变焦范围: %.1f - %.1f\n", START_SCALE, END_SCALE);
    
    // 分配内存
    uint8_t* original_y = (uint8_t*)malloc(WIDTH * HEIGHT);
    uint8_t* original_u = (uint8_t*)malloc(WIDTH * HEIGHT / 2);
    uint8_t* original_v = (uint8_t*)malloc(WIDTH * HEIGHT / 2);
    
    uint8_t* output_y = (uint8_t*)malloc(WIDTH * HEIGHT);
    uint8_t* output_u = (uint8_t*)malloc(WIDTH * HEIGHT / 2);
    uint8_t* output_v = (uint8_t*)malloc(WIDTH * HEIGHT / 2);
    
    if (!original_y || !original_u || !original_v || 
        !output_y || !output_u || !output_v) {
        printf("内存分配失败\n");
        return -1;
    }
    
    printf("生成测试图像...\n");
    generate_test_image(original_y, original_u, original_v);
    
    // 创建输出文件（用于保存最后一帧处理结果）
    FILE* fp_output = fopen("zoomed.yuv", "wb");
    if (!fp_output) {
        printf("无法创建输出文件\n");
        return -1;
    }
    
    clock_t start_time, end_time;
    double cpu_time_used;
    double total_time = 0.0;
    
    printf("开始处理 %d 帧...\n", TOTAL_FRAMES);
    
    // 处理每一帧
    for (int frame = 0; frame < TOTAL_FRAMES; frame++) {
        // 计算当前帧的缩放因子 (从1.0到8.0)
        float scale_factor = START_SCALE + (END_SCALE - START_SCALE) * frame / (TOTAL_FRAMES - 1);
        
        start_time = clock();
        
        // 执行数码变焦处理
        int result = process_zoom_frame(
            original_y, original_u, original_v,
            output_y, output_u, output_v,
            scale_factor
        );
        
        end_time = clock();
        cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
        total_time += cpu_time_used;
        
        if (result != 0) {
            printf("第 %d 帧处理失败，错误代码: %d\n", frame + 1, result);
            break;
        }
        
        // 打印每一帧的执行时间
        printf("帧 %d/%d: 缩放因子 = %.3f, 执行时间 = %.6f 秒\n", 
               frame + 1, TOTAL_FRAMES, scale_factor, cpu_time_used);
        
        // 保存最后一帧到文件
        if (frame == TOTAL_FRAMES - 1) {
            fwrite(output_y, WIDTH * HEIGHT, 1, fp_output);
            fwrite(output_u, WIDTH * HEIGHT / 2, 1, fp_output);
            fwrite(output_v, WIDTH * HEIGHT / 2, 1, fp_output);
        }
    }
    
    fclose(fp_output);
    
    printf("\n处理完成!\n");
    printf("总帧数: %d\n", TOTAL_FRAMES);
    printf("总处理时间: %.6f 秒\n", total_time);
    printf("平均每帧处理时间: %.6f 秒\n", total_time / TOTAL_FRAMES);
    printf("输出文件: zoomed.yuv (%dx%d)\n", WIDTH, HEIGHT);
    
    // 释放内存
    free(original_y);
    free(original_u);
    free(original_v);
    free(output_y);
    free(output_u);
    free(output_v);
    
    return 0;
}