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

## 🚀 快速启动

### 方式一：Docker（推荐）

```bash
# 构建并启动
docker compose up --build

# 访问地址
# 浏览器打开: http://localhost:6080
# VNC密码: radar123
```

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

## 📁 项目结构

```
RadarPPI/
├── RadarPPI.pro          # Qt项目配置文件
├── main.cpp              # 程序入口
├── mainwindow.h          # 主窗口头文件
├── mainwindow.cpp        # 主窗口实现
├── mainwindow.ui         # 主窗口UI设计
├── radarwidget.h         # 雷达控件头文件
└── radarwidget.cpp       # 雷达控件实现（核心绘制逻辑）
```

## 🎨 视觉设计

### 配色方案

| 颜色 | 色值 | 用途 |
|------|------|------|
| Primary Green | `#00FF41` | 扫描线、目标高亮 |
| Medium Green | `#00AA2A` | 距离环、刻度 |
| Dim Green | `#007A1E` | 副刻度、余辉 |
| Background | `#0A0F0A` | 背景底色 |

### 界面布局

```
┌────────────────────────────────┐
│                                │
│     ╭────────────────────╮     │
│     │                    │     │
│     │   雷达PPI显示区域   │     │
│     │      600x600       │     │
│     │                    │     │
│     ╰────────────────────╯     │
│                                │
│  ┌────────────────────────┐    │
│  │ [启动] [停止] [重置]   │    │
│  └────────────────────────┘    │
│  ┌────────────────────────┐    │
│  │ 方位: 45.0° │ 运行中   │    │
│  └────────────────────────┘    │
└────────────────────────────────┘
```

## 🔧 技术栈

- **框架**：Qt 5.12.0
- **语言**：C++11
- **图形**：QPainter, QRadialGradient, QConicalGradient
- **动画**：QTimer (60 FPS)
- **容器**：Docker + NoVNC

## 📄 文档

- [需求规格说明书](docs/Requirements.md)
- [开发路线图](docs/Roadmap.md)
- [视觉设计规格](docs/DesignSpec.md)

## 📜 License

MIT License

---

**Alkaid-SOP Project** | Built with ❤️ and Qt
