/**
 * @file trackcontrolpanel.h
 * @brief 轨迹回放控制面板类声明
 */

#ifndef TRACKCONTROLPANEL_H
#define TRACKCONTROLPANEL_H

#include <QWidget>

namespace Ui {
class TrackControlPanel;
}

/**
 * @class TrackControlPanel
 * @brief 轨迹回放控制面板
 *
 * 功能:
 * - 提供轨迹文件加载入口
 * - 播放/暂停/停止控制
 * - 进度条显示与拖动
 * - 倍速选择 (1x/2x/4x)
 * - 通过信号槽机制与 TrackPlayer 通信
 */
class TrackControlPanel : public QWidget
{
    Q_OBJECT

public:
    explicit TrackControlPanel(QWidget *parent = nullptr);
    ~TrackControlPanel();

public slots:
    /**
     * @brief 更新轨迹加载状态
     * @param success 是否加载成功
     * @param message 加载消息
     */
    void onTrackLoaded(bool success, const QString &message);

    /**
     * @brief 更新播放状态
     * @param isPlaying 是否正在播放
     */
    void onPlayStateChanged(bool isPlaying);

    /**
     * @brief 更新播放时间
     * @param currentTime 当前时间 (毫秒)
     * @param totalDuration 总时长 (毫秒)
     */
    void onTimeChanged(qint64 currentTime, qint64 totalDuration);

    /**
     * @brief 更新播放速度
     * @param speed 当前倍速
     */
    void onSpeedChanged(double speed);

    /**
     * @brief 回放完成处理
     */
    void onPlaybackFinished();

signals:
    /**
     * @brief 请求加载轨迹文件信号
     * @param filePath 文件路径
     */
    void loadTrackRequested(const QString &filePath);

    /**
     * @brief 请求播放信号
     */
    void playRequested();

    /**
     * @brief 请求暂停信号
     */
    void pauseRequested();

    /**
     * @brief 请求停止信号
     */
    void stopRequested();

    /**
     * @brief 请求设置倍速信号
     * @param speed 倍速
     */
    void speedRequested(double speed);

    /**
     * @brief 请求跳转到指定时间信号
     * @param time 时间位置 (毫秒)
     */
    void seekRequested(qint64 time);

private slots:
    /**
     * @brief 加载按钮点击处理
     */
    void onLoadClicked();

    /**
     * @brief 播放按钮点击处理
     */
    void onPlayClicked();

    /**
     * @brief 暂停按钮点击处理
     */
    void onPauseClicked();

    /**
     * @brief 停止按钮点击处理
     */
    void onStopClicked();

    /**
     * @brief 倍速选择变化处理
     * @param index 选择索引
     */
    void onSpeedIndexChanged(int index);

    /**
     * @brief 进度条拖动处理
     * @param value 进度值
     */
    void onSliderValueChanged(int value);

    /**
     * @brief 进度条释放处理
     */
    void onSliderReleased();

private:
    /**
     * @brief 连接内部信号槽
     */
    void connectInternalSignals();

    /**
     * @brief 格式化时间显示
     * @param milliseconds 毫秒数
     * @return 格式化的时间字符串
     */
    QString formatTime(qint64 milliseconds) const;

private:
    Ui::TrackControlPanel *ui;
    qint64 m_totalDuration;      ///< 轨迹总时长
    bool m_isPlaying;            ///< 是否正在播放
    bool m_isDraggingSlider;     ///< 是否正在拖动滑块
};

#endif // TRACKCONTROLPANEL_H
