# RadarPPI - 雷达P型显示仿真系统

<p align="center">
  <img src="https://img.shields.io/badge/Qt-5.12.0-green?style=flat-square&logo=qt" />
  <img src="https://img.shields.io/badge/C++-11-blue?style=flat-square&logo=cplusplus" />
  <img src="https://img.shields.io/badge/Docker-Ready-blue?style=flat-square&logo=docker" />
</p>

## 📖 项目简介

RadarPPI 是一个基于 Qt 5.12.0 的雷达P型显示（Plan Position Indicator）仿真系统。实现了逼真的雷达扫描效果、余辉渐变和目标显示功能，采用经典军用雷达绿色主题设计。

## ✨ 功能特性

- **雷达背景绘制**：同心圆距离环、方位刻度线、十字准线
- **扫描线动画**：顺时针旋转扫描，可调节转速（默认10 RPM）
- **扫描余辉效果**：90°角度渐变，模拟CRT荧光衰减
- **目标显示**：多目标点显示，带发光效果和余辉衰减
- **控制面板**：启动/停止/重置控制
- **状态显示**：实时方位角、运行状态、转速信息

---

## 🚀 快速启动

### 方式一：Docker 一键启动（推荐）

```bash
# 构建并启动
docker compose up --build

# 后台运行
docker compose up -d --build
```

**启动成功后：**
- 🌐 浏览器访问：**http://localhost:6080**
- 🔑 VNC 密码：**radar123**
- 点击 "Connect" 按钮连接

### 方式二：本地编译

**前置要求**：Qt 5.12.0 或更高版本

```bash
cd RadarPPI

# 生成 Makefile
qmake RadarPPI.pro

# 编译
make

# 运行
./RadarPPI
```

---

## 📁 项目结构

```
label-00997/
├── .alkaid-sop              # 协议标识文件
├── .gitignore               # Git 忽略配置
├── Dockerfile               # Docker 镜像构建
├── docker-compose.yml       # Docker Compose 配置
├── README.md                # 项目说明文档
│
├── RadarPPI/                # Qt 项目源码
│   ├── RadarPPI.pro         # Qt 项目配置文件
│   ├── main.cpp             # 程序入口
│   ├── mainwindow.h         # 主窗口头文件
│   ├── mainwindow.cpp       # 主窗口实现
│   ├── mainwindow.ui        # 主窗口 UI 设计
│   ├── radarwidget.h        # 雷达控件头文件
│   └── radarwidget.cpp      # 雷达控件实现（核心绘制）
│
├── docker/                  # Docker 配置
│   ├── start.sh             # 容器启动脚本
│   └── supervisord.conf     # 进程管理配置
│
└── docs/                    # 项目文档
    ├── Requirements.md      # 需求规格说明书
    ├── Roadmap.md           # 开发路线图
    └── DesignSpec.md        # 视觉设计规格
```

---

## 🎨 视觉设计

### 配色方案

| 颜色 | 色值 | 用途 |
|------|------|------|
| Primary Green | `#00FF41` | 扫描线、目标高亮 |
| Bright Green | `#00D634` | 目标点 |
| Medium Green | `#00AA2A` | 距离环、刻度标注 |
| Dim Green | `#007A1E` | 副刻度、弱光 |
| Background | `#0A0F0A` | 背景底色 |

### 界面布局

```
┌────────────────────────────────────────┐
│                                        │
│         ╭──────────────────╮           │
│         │        N         │           │
│         │    ╲   │   ╱     │           │
│         │     ╲  │  ╱ ·    │           │
│         │   W ───⊙─── E    │  600x600  │
│         │       ╱ ╲        │           │
│         │      ╱   ╲       │           │
│         │        S         │           │
│         ╰──────────────────╯           │
│                                        │
│  ┌──────────────────────────────────┐  │
│  │  [▶ 启动]  [⏹ 停止]  [↺ 重置]   │  │
│  └──────────────────────────────────┘  │
│  ┌──────────────────────────────────┐  │
│  │ 方位: 127.5° │ 运行中 │ 10 RPM  │  │
│  └──────────────────────────────────┘  │
└────────────────────────────────────────┘
```

---

## 🔧 技术实现

### 核心技术栈

| 组件 | 技术 |
|------|------|
| GUI 框架 | Qt 5.12.0 Widgets |
| 编程语言 | C++11 |
| 2D 绑制 | QPainter |
| 动画驱动 | QTimer (60 FPS) |
| 扫描余辉 | QConicalGradient |
| 目标光晕 | QRadialGradient |
| 容器化 | Docker + NoVNC |

### 关键类说明

| 类 | 职责 |
|------|------|
| `MainWindow` | 主窗口，布局管理，控制逻辑 |
| `RadarWidget` | 雷达PPI绑制、动画、目标管理 |
| `RadarTarget` | 目标数据结构 |

---

## 🐳 Docker 部署说明

### 环境变量

| 变量 | 默认值 | 说明 |
|------|--------|------|
| `VNC_RESOLUTION` | 1280x800 | VNC 分辨率 |
| `VNC_PW` | radar123 | VNC 密码 |
| `DISPLAY` | :1 | X Display |

### 端口映射

| 端口 | 服务 |
|------|------|
| 6080 | NoVNC Web 访问 |
| 5901 | VNC 直连 |

### 常用命令

```bash
# 构建镜像
docker compose build

# 启动服务
docker compose up

# 后台启动
docker compose up -d

# 查看日志
docker compose logs -f

# 停止服务
docker compose down

# 重新构建并启动
docker compose up --build
```

---

## 📜 许可证

MIT License

---

## 📚 相关文档

- [需求规格说明书](docs/Requirements.md)
- [开发路线图](docs/Roadmap.md)
- [视觉设计规格](docs/DesignSpec.md)

---

<p align="center">
  <b>Alkaid-SOP Project</b> | Built with ❤️ and Qt
</p>
