# Docker 存储管理指南

## 查看Docker存储位置

根据系统信息，Docker默认存储位置在 `/var/lib/docker`，当前已使用27GB空间。

## 更改Docker存储位置

如果需要更改Docker的默认存储位置，可以按照以下步骤操作：

### 方法一：修改Docker配置文件（推荐）

1. 停止Docker服务：
   ```bash
   sudo systemctl stop docker
   ```

2. 创建新的存储目录（例如在/home下）：
   ```bash
   sudo mkdir -p /home/docker
   ```

3. 复制现有数据到新位置：
   ```bash
   sudo rsync -aqxP /var/lib/docker/ /home/docker/
   ```

4. 创建或编辑Docker配置文件：
   ```bash
   sudo nano /etc/docker/daemon.json
   ```

5. 添加以下内容：
   ```json
   {
     "data-root": "/home/docker"
   }
   ```

6. 启动Docker服务：
   ```bash
   sudo systemctl start docker
   ```

### 方法二：使用符号链接

1. 停止Docker服务：
   ```bash
   sudo systemctl stop docker
   ```

2. 创建新的存储目录：
   ```bash
   sudo mkdir -p /home/docker
   ```

3. 移动现有数据：
   ```bash
   sudo mv /var/lib/docker/* /home/docker/
   ```

4. 创建符号链接：
   ```bash
   sudo ln -s /home/docker /var/lib/docker
   ```

5. 启动Docker服务：
   ```bash
   sudo systemctl start docker
   ```

## 清理Docker占用空间

### 清理未使用的镜像、容器和缓存
```bash
# 清理所有未使用的镜像、容器、卷和网络
docker system prune -a

# 只清理未使用的镜像
docker image prune -a

# 清理未使用的容器
docker container prune

# 清理未使用的卷
docker volume prune
```

### 删除特定镜像
```bash
# 查看镜像列表
docker images

# 删除特定镜像
docker rmi IMAGE_ID
```

### 清理构建缓存
```bash
# 清理构建缓存
docker builder prune
```

## 针对本项目优化建议

1. 构建完成后及时清理镜像：
   ```bash
   docker rmi libyuv-win-build
   ```

2. 使用.dockerignore文件减少构建上下文大小（如果存在大量不必要的文件）

3. 定期执行清理操作：
   ```bash
   docker system prune -f
   ```