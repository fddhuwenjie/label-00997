#!/bin/bash
# =============================================================================
# RadarPPI 启动脚本
# =============================================================================

echo "=============================================="
echo "  RadarPPI - 雷达P型显示仿真系统"
echo "=============================================="
echo ""

# 等待 X 服务启动
sleep 3

# 启动雷达应用
echo "[Startup] Launching RadarPPI..."
cd /app/RadarPPI
export DISPLAY=:1
./RadarPPI &

sleep 2

echo ""
echo "=============================================="
echo "  ✓ Startup Success"
echo "=============================================="
echo "  Frontend: http://localhost:6080"
echo "  VNC Direct: localhost:5901"
echo "  VNC Password: ${VNC_PW:-radar123}"
echo "=============================================="
echo ""

# 保持脚本运行
tail -f /dev/null
