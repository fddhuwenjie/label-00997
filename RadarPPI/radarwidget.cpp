/**
 * @file radarwidget.cpp
 * @brief 雷达PPI显示控件类实现
 */

#include "radarwidget.h"

#include <QPainter>
#include <QPainterPath>
#include <QRadialGradient>
#include <QConicalGradient>
#include <QtMath>
#include <QResizeEvent>
#include <QRandomGenerator>

RadarWidget::RadarWidget(QWidget *parent)
    : QWidget(parent)
    , m_timer(new QTimer(this))
    , m_currentAngle(0.0)
    , m_rpm(10)
    , m_isScanning(false)
    , m_radius(0.0)
    , m_distanceRings(5)
{
    // 初始化颜色
    m_colorPrimaryGreen = QColor(0, 255, 65);       // #00FF41
    m_colorBrightGreen = QColor(0, 214, 52);        // #00D634
    m_colorMediumGreen = QColor(0, 170, 42);        // #00AA2A
    m_colorDimGreen = QColor(0, 122, 30);           // #007A1E
    m_colorFaintGreen = QColor(0, 74, 18);          // #004A12
    m_colorGhostGreen = QColor(0, 42, 10);          // #002A0A
    m_colorBackground = QColor(10, 15, 10);         // #0A0F0A
    m_colorBackgroundCenter = QColor(13, 20, 13);   // #0D140D

    // 设置背景
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, m_colorBackground);
    setPalette(pal);

    // 启用抗锯齿
    setAttribute(Qt::WA_OpaquePaintEvent, false);

    // 设置最小尺寸
    setMinimumSize(400, 400);

    // 初始化目标
    initTargets();

    // 连接定时器
    connect(m_timer, &QTimer::timeout, this, &RadarWidget::onTimerUpdate);
}

RadarWidget::~RadarWidget()
{
}

void RadarWidget::startScan()
{
    if (!m_isScanning) {
        m_isScanning = true;
        // 60 FPS, 角度增量基于 RPM 计算
        m_timer->start(16); // ~60 FPS
    }
}

void RadarWidget::stopScan()
{
    if (m_isScanning) {
        m_isScanning = false;
        m_timer->stop();
    }
}

void RadarWidget::reset()
{
    stopScan();
    m_currentAngle = 0.0;
    initTargets();
    update();
    emit angleChanged(m_currentAngle);
}

void RadarWidget::setRPM(int rpm)
{
    m_rpm = qBound(1, rpm, 60);
}

void RadarWidget::onTimerUpdate()
{
    // 计算角度增量: RPM -> 度/帧
    // 每分钟转rpm圈，每圈360度，每秒60帧
    // 角度/帧 = (rpm * 360) / (60 * 60) = rpm * 0.1
    double angleIncrement = m_rpm * 0.1;
    m_currentAngle += angleIncrement;

    if (m_currentAngle >= 360.0) {
        m_currentAngle -= 360.0;
    }

    // 更新目标亮度
    updateTargetBrightness();

    // 刷新显示
    update();

    // 发送角度变化信号
    emit angleChanged(m_currentAngle);
}

void RadarWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // 绘制各层
    drawBackground(painter);
    drawSweepTrail(painter);
    drawDistanceRings(painter);
    drawAzimuthLines(painter);
    drawCrossHair(painter);
    drawTargets(painter);
    drawSweepLine(painter);
    drawLabels(painter);
}

void RadarWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    calculateRadius();
}

void RadarWidget::calculateRadius()
{
    // 计算可用的最大正方形区域
    int side = qMin(width(), height());
    m_radius = (side - 60) / 2.0; // 留出边距用于标注
    m_center = QPointF(width() / 2.0, height() / 2.0);
}

QPointF RadarWidget::polarToCartesian(double distance, double angleDeg) const
{
    // 将角度转换为弧度，0度在顶部（北），顺时针增加
    double angleRad = qDegreesToRadians(angleDeg - 90.0);
    double r = distance * m_radius;
    return QPointF(
        m_center.x() + r * qCos(angleRad),
        m_center.y() + r * qSin(angleRad)
    );
}

void RadarWidget::drawBackground(QPainter &painter)
{
    // 绘制径向渐变背景
    QRadialGradient gradient(m_center, m_radius);
    gradient.setColorAt(0.0, m_colorBackgroundCenter);
    gradient.setColorAt(1.0, m_colorBackground);

    painter.setPen(Qt::NoPen);
    painter.setBrush(gradient);
    painter.drawEllipse(m_center, m_radius, m_radius);
}

void RadarWidget::drawDistanceRings(QPainter &painter)
{
    QPen pen(m_colorDimGreen, 1);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    for (int i = 1; i <= m_distanceRings; ++i) {
        double ringRadius = (m_radius * i) / m_distanceRings;
        painter.drawEllipse(m_center, ringRadius, ringRadius);
    }
}

void RadarWidget::drawAzimuthLines(QPainter &painter)
{
    // 主刻度线 (每30度)
    QPen mainPen(m_colorMediumGreen, 2);
    painter.setPen(mainPen);

    for (int angle = 0; angle < 360; angle += 30) {
        QPointF inner = polarToCartesian(0.95, angle);
        QPointF outer = polarToCartesian(1.0, angle);
        painter.drawLine(inner, outer);
    }

    // 副刻度线 (每10度，排除主刻度)
    QPen subPen(m_colorDimGreen, 1);
    painter.setPen(subPen);

    for (int angle = 0; angle < 360; angle += 10) {
        if (angle % 30 != 0) {
            QPointF inner = polarToCartesian(0.97, angle);
            QPointF outer = polarToCartesian(1.0, angle);
            painter.drawLine(inner, outer);
        }
    }
}

void RadarWidget::drawCrossHair(QPainter &painter)
{
    QPen pen(m_colorFaintGreen, 1, Qt::DashLine);
    pen.setDashPattern(QVector<qreal>() << 5 << 5);
    painter.setPen(pen);

    // 水平线
    painter.drawLine(
        QPointF(m_center.x() - m_radius, m_center.y()),
        QPointF(m_center.x() + m_radius, m_center.y())
    );

    // 垂直线
    painter.drawLine(
        QPointF(m_center.x(), m_center.y() - m_radius),
        QPointF(m_center.x(), m_center.y() + m_radius)
    );
}

void RadarWidget::drawLabels(QPainter &painter)
{
    QFont font("Consolas", 9);
    painter.setFont(font);
    painter.setPen(m_colorMediumGreen);

    // 方位角标注 (N, E, S, W 和角度)
    QStringList directions = {"N", "30", "60", "E", "120", "150", "S", "210", "240", "W", "300", "330"};
    int angleStep = 30;

    for (int i = 0; i < 12; ++i) {
        int angle = i * angleStep;
        QPointF pos = polarToCartesian(1.08, angle);

        QString text = directions[i];
        QFontMetrics fm(font);
        int textWidth = fm.horizontalAdvance(text);
        int textHeight = fm.height();

        painter.drawText(
            pos.x() - textWidth / 2,
            pos.y() + textHeight / 4,
            text
        );
    }

    // 距离标注 (右侧)
    for (int i = 1; i <= m_distanceRings; ++i) {
        double dist = (double)i / m_distanceRings;
        QPointF pos = polarToCartesian(dist, 85); // 稍微偏离正东方向

        QString text = QString("%1").arg(i * 20); // 假设最大100km
        QFontMetrics fm(font);
        int textWidth = fm.horizontalAdvance(text);

        painter.drawText(pos.x() + 5, pos.y() + 4, text);
    }
}

void RadarWidget::drawSweepTrail(QPainter &painter)
{
    if (!m_isScanning && m_currentAngle == 0.0) {
        return;
    }

    // 创建扫描余辉效果 - 使用锥形渐变
    QConicalGradient gradient(m_center, -m_currentAngle + 90);

    // 余辉覆盖90度范围
    gradient.setColorAt(0.0, QColor(0, 255, 65, 180));   // 扫描线位置 - 最亮
    gradient.setColorAt(0.05, QColor(0, 214, 52, 120));
    gradient.setColorAt(0.10, QColor(0, 170, 42, 80));
    gradient.setColorAt(0.15, QColor(0, 122, 30, 50));
    gradient.setColorAt(0.20, QColor(0, 74, 18, 25));
    gradient.setColorAt(0.25, QColor(0, 42, 10, 10));
    gradient.setColorAt(0.30, QColor(0, 42, 10, 0));
    gradient.setColorAt(1.0, QColor(0, 42, 10, 0));

    painter.setPen(Qt::NoPen);
    painter.setBrush(gradient);
    painter.drawEllipse(m_center, m_radius, m_radius);
}

void RadarWidget::drawSweepLine(QPainter &painter)
{
    if (!m_isScanning && m_currentAngle == 0.0) {
        // 绘制静态指示线
        QPen pen(m_colorDimGreen, 2);
        painter.setPen(pen);
        QPointF endPoint = polarToCartesian(1.0, 0);
        painter.drawLine(m_center, endPoint);
        return;
    }

    // 绘制扫描线
    QPointF endPoint = polarToCartesian(1.0, m_currentAngle);

    // 外发光效果
    for (int i = 3; i >= 0; --i) {
        int alpha = 50 - i * 15;
        QPen glowPen(QColor(0, 255, 65, alpha), 2 + i * 2);
        glowPen.setCapStyle(Qt::RoundCap);
        painter.setPen(glowPen);
        painter.drawLine(m_center, endPoint);
    }

    // 主扫描线
    QPen mainPen(m_colorPrimaryGreen, 2);
    mainPen.setCapStyle(Qt::RoundCap);
    painter.setPen(mainPen);
    painter.drawLine(m_center, endPoint);
}

void RadarWidget::initTargets()
{
    m_targets.clear();

    // 生成随机目标
    QRandomGenerator *rng = QRandomGenerator::global();

    // 添加8个随机目标
    for (int i = 0; i < 8; ++i) {
        RadarTarget target;
        target.distance = 0.2 + rng->generateDouble() * 0.7; // 0.2 - 0.9
        target.azimuth = rng->generateDouble() * 360.0;
        target.brightness = 0.0;
        target.lastScanAngle = target.azimuth;
        target.active = true;
        m_targets.append(target);
    }
}

void RadarWidget::updateTargetBrightness()
{
    for (int i = 0; i < m_targets.size(); ++i) {
        RadarTarget &target = m_targets[i];
        if (!target.active) continue;

        // 计算当前扫描线与目标的角度差
        double angleDiff = m_currentAngle - target.azimuth;

        // 归一化到 0-360
        while (angleDiff < 0) angleDiff += 360.0;
        while (angleDiff >= 360.0) angleDiff -= 360.0;

        // 如果扫描线刚扫过目标（角度差很小），刷新亮度
        if (angleDiff < 3.0) {
            target.brightness = 1.0;
            target.lastScanAngle = m_currentAngle;
        } else {
            // 根据角度差衰减亮度
            // 一个完整周期（360度）亮度从1衰减到0.1
            target.brightness = qMax(0.1, 1.0 - angleDiff / 360.0);
        }
    }
}

void RadarWidget::drawTargets(QPainter &painter)
{
    for (const RadarTarget &target : m_targets) {
        if (!target.active || target.brightness < 0.1) continue;

        QPointF pos = polarToCartesian(target.distance, target.azimuth);

        // 目标大小基于距离和亮度
        double baseSize = 6.0 + (1.0 - target.distance) * 4.0;
        double size = baseSize * (0.5 + target.brightness * 0.5);

        // 绘制目标光晕
        QRadialGradient glow(pos, size * 2);
        glow.setColorAt(0.0, QColor(0, 255, 65, int(255 * target.brightness)));
        glow.setColorAt(0.3, QColor(0, 214, 52, int(150 * target.brightness)));
        glow.setColorAt(0.6, QColor(0, 170, 42, int(80 * target.brightness)));
        glow.setColorAt(1.0, QColor(0, 170, 42, 0));

        painter.setPen(Qt::NoPen);
        painter.setBrush(glow);
        painter.drawEllipse(pos, size * 2, size * 2);

        // 绘制目标核心
        painter.setBrush(QColor(0, 255, 65, int(255 * target.brightness)));
        painter.drawEllipse(pos, size * 0.5, size * 0.5);
    }
}
