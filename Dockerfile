# =============================================================================
# RadarPPI - 雷达P型显示仿真系统
# Docker镜像构建文件
# 基于 Ubuntu + Qt5 + NoVNC
# =============================================================================

FROM ubuntu:20.04

# 设置环境变量，避免交互式安装
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Asia/Shanghai

# 安装必要的软件包
RUN apt-get update && apt-get install -y \
    # Qt5 开发库
    qt5-default \
    qtbase5-dev \
    qtbase5-dev-tools \
    # 构建工具
    build-essential \
    # VNC 相关
    x11vnc \
    xvfb \
    fluxbox \
    # NoVNC 依赖
    python3 \
    python3-numpy \
    net-tools \
    # 中文字体支持
    fonts-wqy-zenhei \
    fonts-wqy-microhei \
    # 工具
    supervisor \
    curl \
    git \
    && rm -rf /var/lib/apt/lists/*

# 安装 noVNC 和 websockify
RUN git clone --depth 1 https://github.com/novnc/noVNC.git /opt/novnc \
    && git clone --depth 1 https://github.com/novnc/websockify.git /opt/novnc/utils/websockify \
    && ln -s /opt/novnc/vnc.html /opt/novnc/index.html

# 设置工作目录
WORKDIR /app

# 复制项目文件
COPY RadarPPI/ /app/RadarPPI/

# 编译项目
WORKDIR /app/RadarPPI
RUN qmake RadarPPI.pro && make -j$(nproc)

# 复制启动脚本
COPY docker/start.sh /app/start.sh
COPY docker/supervisord.conf /etc/supervisor/conf.d/supervisord.conf
RUN chmod +x /app/start.sh

# 设置显示环境
ENV DISPLAY=:1
ENV VNC_RESOLUTION=1280x800
ENV VNC_PW=radar123

# 暴露端口
EXPOSE 5901 6080

# 启动命令
CMD ["/usr/bin/supervisord", "-c", "/etc/supervisor/conf.d/supervisord.conf"]
