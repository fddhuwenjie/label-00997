/**
 * @file trackplayer.h
 * @brief 目标轨迹回放播放器类声明
 */

#ifndef TRACKPLAYER_H
#define TRACKPLAYER_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QString>
#include <algorithm>

/**
 * @struct TrackPoint
 * @brief 轨迹点数据结构
 */
struct TrackPoint {
    double timestamp;  ///< 时间戳 (秒)
    double azimuth;    ///< 方位角 (度)
    double distance;   ///< 距离 (km)
    int targetId;      ///< 目标ID

    TrackPoint()
        : timestamp(0.0), azimuth(0.0), distance(0.0), targetId(0) {}

    TrackPoint(double ts, double azi, double dist, int id)
        : timestamp(ts), azimuth(azi), distance(dist), targetId(id) {}
};

/**
 * @class TrackPlayer
 * @brief 目标轨迹回放播放器
 *
 * 从JSON文件加载预录制的目标运动轨迹数据，按时间顺序回放
 * 支持播放/暂停/倍速控制(1x/2x/4x)
 * 通过Qt信号槽机制与雷达显示组件通信
 */
class TrackPlayer : public QObject
{
    Q_OBJECT

public:
    enum PlaybackSpeed {
        Speed1x = 1,
        Speed2x = 2,
        Speed4x = 4
    };

    explicit TrackPlayer(QObject *parent = nullptr);
    ~TrackPlayer();

    /**
     * @brief 从JSON文件加载轨迹数据
     * @param filePath JSON文件路径
     * @return 是否加载成功
     */
    bool loadFromFile(const QString &filePath);

    /**
     * @brief 开始回放
     */
    void play();

    /**
     * @brief 暂停回放
     */
    void pause();

    /**
     * @brief 停止回放并重置
     */
    void stop();

    /**
     * @brief 设置回放速度
     * @param speed 速度倍率
     */
    void setPlaybackSpeed(PlaybackSpeed speed);

    /**
     * @brief 获取当前回放速度
     * @return 速度倍率
     */
    PlaybackSpeed playbackSpeed() const { return m_speed; }

    /**
     * @brief 是否正在回放
     * @return 回放状态
     */
    bool isPlaying() const { return m_isPlaying; }

    /**
     * @brief 获取加载的轨迹数据
     * @return 轨迹点列表
     */
    const QVector<TrackPoint> &trackPoints() const { return m_trackPoints; }

    /**
     * @brief 获取当前回放时间
     * @return 当前时间 (秒)
     */
    double currentTime() const { return m_currentTime; }

    /**
     * @brief 获取轨迹总时长
     * @return 总时长 (秒)
     */
    double totalDuration() const { return m_totalDuration; }

signals:
    /**
     * @brief 目标更新信号
     * @param targetId 目标ID
     * @param azimuth 方位角
     * @param distance 距离
     */
    void targetUpdated(int targetId, double azimuth, double distance);

    /**
     * @brief 回放开始信号
     */
    void playbackStarted();

    /**
     * @brief 回放暂停信号
     */
    void playbackPaused();

    /**
     * @brief 回放停止信号
     */
    void playbackStopped();

    /**
     * @brief 回放完成信号
     */
    void playbackFinished();

    /**
     * @brief 进度更新信号
     * @param current 当前时间
     * @param total 总时长
     */
    void progressChanged(double current, double total);

    /**
     * @brief 错误信号
     * @param errorMsg 错误信息
     */
    void errorOccurred(const QString &errorMsg);

private slots:
    /**
     * @brief 定时器更新槽
     */
    void onTimerUpdate();

private:
    /**
     * @brief 解析JSON数据
     * @param jsonData JSON字符串
     * @return 是否解析成功
     */
    bool parseJson(const QByteArray &jsonData);

    /**
     * @brief 发出当前时刻之前所有目标更新
     */
    void emitUpdatesUpToCurrentTime();

    QTimer *m_timer;
    QVector<TrackPoint> m_trackPoints;
    int m_currentIndex;
    double m_currentTime;
    double m_totalDuration;
    bool m_isPlaying;
    PlaybackSpeed m_speed;
};

#endif // TRACKPLAYER_H
