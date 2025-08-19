# LibYUV Windows交叉编译工具

本项目提供了一个基于Docker的环境，用于在Linux系统上交叉编译LibYUV库，生成适用于Windows平台的x86和x64版本。

## 项目结构

```
.
├── libyuv-src/           # LibYUV源码目录
├── Dockerfile            # Docker镜像构建文件
├── build.sh              # 编译脚本
└── README.md             # 说明文档
```

## 环境要求

- Docker 18.09或更高版本
- Linux系统（推荐Ubuntu 22.04+或其他主流发行版）

## 构建Docker镜像

```bash
docker build -t libyuv-win-build .
```

## 运行编译

创建输出目录并运行容器进行编译：

```bash
# 创建本地输出目录
mkdir -p output

# 运行编译
docker run --rm -v $(pwd)/output:/opt/output libyuv-win-build
```

编译完成后，生成的文件将位于本地的`output`目录中：

```
output/
├── x86/
│   ├── include/          # 头文件
│   └── *.a               # 静态库文件
└── x64/
    ├── include/          # 头文件
    └── *.a               # 静态库文件
```

## 在Windows项目中使用

1. 将对应架构的头文件和库文件复制到您的Windows项目中
2. 在Visual Studio或其他开发环境中配置包含目录和库目录
3. 链接相应的库文件

## 自定义编译选项

如需修改编译选项，可以编辑[build.sh](build.sh)文件，例如：

- 更改编译优化级别
- 添加其他编译选项
- 更改输出目录结构

## 故障排除

### 权限问题

如果遇到权限问题，请确保运行Docker命令的用户具有足够权限，或使用`sudo`运行。

### 编译错误

如果编译过程中出现错误，请检查：
1. Docker镜像是否构建成功
2. libyuv源码是否完整
3. 系统资源是否充足

## 许可证

LibYUV使用BSD-style许可证。详情请参见libyuv-src/LICENSE文件。


## 运行测试程序生成的文件
> ffplay -f rawvideo -pixel_format yuyv422 -video_size 1920x1080 -framerate 60 output/zoomed_all_frames.yuv