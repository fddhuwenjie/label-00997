/**
 * @file trackplayer.h
 * @brief 目标轨迹回放控制器类声明
 */

#ifndef TRACKPLAYER_H
#define TRACKPLAYER_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QMap>
#include <QString>

/**
 * @struct TrackPoint
 * @brief 轨迹点数据结构
 */
struct TrackPoint {
    qint64 timestamp;      ///< 时间戳 (毫秒)
    double azimuth;        ///< 方位角 (度)
    double distance;       ///< 距离 (归一化 0.0 - 1.0)
    QString targetId;      ///< 目标ID

    TrackPoint()
        : timestamp(0), azimuth(0.0), distance(0.0) {}

    TrackPoint(qint64 ts, double azi, double dist, const QString &id)
        : timestamp(ts), azimuth(azi), distance(dist), targetId(id) {}
};

/**
 * @struct ActiveTrackTarget
 * @brief 当前活跃的回放目标
 */
struct ActiveTrackTarget {
    QString targetId;      ///< 目标ID
    double azimuth;        ///< 当前方位角
    double distance;       ///< 当前距离
    double brightness;     ///< 亮度
    bool active;           ///< 是否激活

    ActiveTrackTarget()
        : azimuth(0.0), distance(0.0), brightness(1.0), active(false) {}

    ActiveTrackTarget(const QString &id, double azi, double dist)
        : targetId(id), azimuth(azi), distance(dist), brightness(1.0), active(true) {}
};

/**
 * @class TrackPlayer
 * @brief 目标轨迹回放控制器
 *
 * 功能:
 * - 从 JSON 文件加载轨迹数据
 * - 按时间顺序回放目标运动
 * - 支持播放/暂停/停止控制
 * - 支持 1x/2x/4x 倍速播放
 * - 通过信号槽机制与雷达显示组件通信
 */
class TrackPlayer : public QObject
{
    Q_OBJECT

public:
    explicit TrackPlayer(QObject *parent = nullptr);
    ~TrackPlayer();

    /**
     * @brief 从 JSON 文件加载轨迹数据
     * @param filePath JSON 文件路径
     * @return 是否加载成功
     */
    bool loadFromFile(const QString &filePath);

    /**
     * @brief 获取加载状态
     * @return 是否已加载轨迹数据
     */
    bool isLoaded() const { return m_isLoaded; }

    /**
     * @brief 获取轨迹总时长
     * @return 总时长 (毫秒)
     */
    qint64 getTotalDuration() const { return m_totalDuration; }

    /**
     * @brief 获取当前播放位置
     * @return 当前时间 (毫秒)
     */
    qint64 getCurrentTime() const { return m_currentTime; }

    /**
     * @brief 获取当前播放速度
     * @return 倍速 (1.0, 2.0, 4.0)
     */
    double getSpeed() const { return m_speed; }

    /**
     * @brief 获取播放状态
     * @return 是否正在播放
     */
    bool isPlaying() const { return m_isPlaying; }

    /**
     * @brief 获取轨迹中的目标数量
     * @return 目标数量
     */
    int getTargetCount() const { return m_targetIds.size(); }

    /**
     * @brief 获取所有目标ID
     * @return 目标ID列表
     */
    QStringList getTargetIds() const { return m_targetIds; }

public slots:
    /**
     * @brief 开始/继续播放
     */
    void play();

    /**
     * @brief 暂停播放
     */
    void pause();

    /**
     * @brief 停止播放并重置到开始位置
     */
    void stop();

    /**
     * @brief 设置播放倍速
     * @param speed 倍速 (1.0, 2.0, 4.0)
     */
    void setSpeed(double speed);

    /**
     * @brief 跳转到指定时间位置
     * @param time 时间位置 (毫秒)
     */
    void seek(qint64 time);

signals:
    /**
     * @brief 轨迹数据加载完成信号
     * @param success 是否加载成功
     * @param message 加载结果消息
     */
    void trackLoaded(bool success, const QString &message);

    /**
     * @brief 播放状态变化信号
     * @param isPlaying 是否正在播放
     */
    void playStateChanged(bool isPlaying);

    /**
     * @brief 当前播放时间变化信号
     * @param currentTime 当前时间 (毫秒)
     * @param totalDuration 总时长 (毫秒)
     */
    void timeChanged(qint64 currentTime, qint64 totalDuration);

    /**
     * @brief 播放速度变化信号
     * @param speed 当前倍速
     */
    void speedChanged(double speed);

    /**
     * @brief 回放目标更新信号
     * @param targets 当前活跃的目标列表
     */
    void targetsUpdated(const QVector<ActiveTrackTarget> &targets);

    /**
     * @brief 回放完成信号
     */
    void playbackFinished();

private slots:
    /**
     * @brief 定时器更新槽
     */
    void onTimerUpdate();

private:
    /**
     * @brief 更新当前时间点的目标状态
     */
    void updateTargetsAtCurrentTime();

    /**
     * @brief 解析目标在指定时间点的位置
     * @param targetId 目标ID
     * @param time 时间点
     * @param outAzimuth 输出方位角
     * @param outDistance 输出距离
     * @return 是否找到有效位置
     */
    bool interpolateTargetPosition(const QString &targetId, qint64 time,
                                   double &outAzimuth, double &outDistance);

private:
    QTimer *m_timer;                   ///< 回放定时器
    bool m_isLoaded;                   ///< 是否已加载轨迹
    bool m_isPlaying;                  ///< 是否正在播放
    double m_speed;                    ///< 播放倍速
    qint64 m_currentTime;              ///< 当前播放时间
    qint64 m_totalDuration;            ///< 轨迹总时长
    qint64 m_lastUpdateTime;           ///< 上次更新时间

    QStringList m_targetIds;           ///< 所有目标ID列表
    QMap<QString, QVector<TrackPoint>> m_trackData;  ///< 按目标ID分组的轨迹数据
    QVector<ActiveTrackTarget> m_activeTargets;      ///< 当前活跃目标
};

#endif // TRACKPLAYER_H
