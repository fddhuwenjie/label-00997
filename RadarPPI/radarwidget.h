/**
 * @file radarwidget.h
 * @brief 雷达PPI显示控件类声明
 */

#ifndef RADARWIDGET_H
#define RADARWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QVector>
#include <QPointF>
#include <QString>

#include "trackplayer.h"

/**
 * @struct RadarTarget
 * @brief 雷达目标数据结构
 */
struct RadarTarget {
    double distance;      ///< 距离 (0.0 - 1.0, 归一化)
    double azimuth;       ///< 方位角 (度)
    double brightness;    ///< 当前亮度 (0.0 - 1.0)
    double lastScanAngle; ///< 上次被扫描时的角度
    bool active;          ///< 是否激活

    RadarTarget()
        : distance(0.5), azimuth(0.0), brightness(1.0),
          lastScanAngle(0.0), active(true) {}

    RadarTarget(double dist, double azi)
        : distance(dist), azimuth(azi), brightness(1.0),
          lastScanAngle(0.0), active(true) {}
};

/**
 * @class RadarWidget
 * @brief 雷达PPI显示控件
 *
 * 实现功能:
 * - 雷达背景绘制 (距离环、方位线、刻度)
 * - 扫描线旋转动画
 * - 扫描余辉效果
 * - 目标显示与余辉衰减
 */
class RadarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RadarWidget(QWidget *parent = nullptr);
    ~RadarWidget();

    /**
     * @brief 启动扫描
     */
    void startScan();

    /**
     * @brief 停止扫描
     */
    void stopScan();

    /**
     * @brief 重置状态
     */
    void reset();

    /**
     * @brief 获取当前转速
     * @return 转速 (RPM)
     */
    int getRPM() const { return m_rpm; }

    /**
     * @brief 设置转速
     * @param rpm 转速 (RPM)
     */
    void setRPM(int rpm);

    /**
     * @brief 设置轨迹回放模式
     * @param enabled 是否启用回放模式
     */
    void setTrackPlaybackMode(bool enabled);

    /**
     * @brief 获取是否处于回放模式
     * @return 是否处于回放模式
     */
    bool isTrackPlaybackMode() const { return m_trackPlaybackMode; }

signals:
    /**
     * @brief 扫描角度变化信号
     * @param angle 当前角度
     */
    void angleChanged(double angle);

public slots:
    /**
     * @brief 更新回放目标数据
     * @param targets 回放目标列表
     */
    void onTrackTargetsUpdated(const QVector<ActiveTrackTarget> &targets);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    /**
     * @brief 定时器更新槽
     */
    void onTimerUpdate();

private:
    /**
     * @brief 绘制背景
     */
    void drawBackground(QPainter &painter);

    /**
     * @brief 绘制距离环
     */
    void drawDistanceRings(QPainter &painter);

    /**
     * @brief 绘制方位刻度线
     */
    void drawAzimuthLines(QPainter &painter);

    /**
     * @brief 绘制十字准线
     */
    void drawCrossHair(QPainter &painter);

    /**
     * @brief 绘制刻度标注
     */
    void drawLabels(QPainter &painter);

    /**
     * @brief 绘制扫描余辉
     */
    void drawSweepTrail(QPainter &painter);

    /**
     * @brief 绘制扫描线
     */
    void drawSweepLine(QPainter &painter);

    /**
     * @brief 绘制目标点
     */
    void drawTargets(QPainter &painter);

    /**
     * @brief 初始化模拟目标
     */
    void initTargets();

    /**
     * @brief 更新目标亮度
     */
    void updateTargetBrightness();

    /**
     * @brief 计算显示半径
     */
    void calculateRadius();

    /**
     * @brief 极坐标转笛卡尔坐标
     */
    QPointF polarToCartesian(double distance, double angleDeg) const;

private:
    // 定时器
    QTimer *m_timer;

    // 扫描参数
    double m_currentAngle;     ///< 当前扫描角度 (度)
    int m_rpm;                 ///< 转速 (RPM)
    bool m_isScanning;         ///< 是否正在扫描

    // 显示参数
    QPointF m_center;          ///< 中心点
    double m_radius;           ///< 显示半径
    int m_distanceRings;       ///< 距离环数量

    // 目标数据
    QVector<RadarTarget> m_targets;

    // 轨迹回放目标
    QVector<ActiveTrackTarget> m_trackTargets;
    bool m_trackPlaybackMode;

    // 颜色常量
    QColor m_colorPrimaryGreen;
    QColor m_colorBrightGreen;
    QColor m_colorMediumGreen;
    QColor m_colorDimGreen;
    QColor m_colorFaintGreen;
    QColor m_colorGhostGreen;
    QColor m_colorBackground;
    QColor m_colorBackgroundCenter;
};

#endif // RADARWIDGET_H
