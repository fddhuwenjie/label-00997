#!/bin/bash
# =============================================================================
# RadarPPI 启动脚本
# =============================================================================

echo ""
echo "╔══════════════════════════════════════════════════════════════╗"
echo "║         RadarPPI - 雷达P型显示仿真系统                        ║"
echo "║         Radar Plan Position Indicator Simulator              ║"
echo "╚══════════════════════════════════════════════════════════════╝"
echo ""

# 等待 X 服务启动
echo "[INFO] Waiting for X server..."
sleep 3

# 检查 X 服务
if xdpyinfo -display :1 >/dev/null 2>&1; then
    echo "[INFO] X server is ready"
else
    echo "[WARN] X server may not be fully ready, continuing..."
fi

# 启动雷达应用
echo "[INFO] Launching RadarPPI application..."
cd /app/RadarPPI
export DISPLAY=:1
./RadarPPI &

# 等待应用启动
sleep 2

# 检查应用是否运行
if pgrep -x "RadarPPI" > /dev/null; then
    echo "[INFO] RadarPPI process started successfully"
else
    echo "[WARN] RadarPPI process status unknown"
fi

echo ""
echo "╔══════════════════════════════════════════════════════════════╗"
echo "║                    ✓ Startup Success                         ║"
echo "╠══════════════════════════════════════════════════════════════╣"
echo "║  Frontend URL  :  http://localhost:6080                      ║"
echo "║  VNC Direct    :  localhost:5901                             ║"
echo "║  VNC Password  :  ${VNC_PW:-radar123}                                       ║"
echo "╠══════════════════════════════════════════════════════════════╣"
echo "║  Open your browser and navigate to http://localhost:6080     ║"
echo "║  Click 'Connect' and enter password when prompted            ║"
echo "╚══════════════════════════════════════════════════════════════╝"
echo ""

# 保持脚本运行
tail -f /dev/null
