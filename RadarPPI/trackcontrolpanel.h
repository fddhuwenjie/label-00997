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
 * 提供播放/暂停/停止按钮、倍速选择、文件加载
 * 通过Qt信号槽机制与TrackPlayer通信
 */
class TrackControlPanel : public QWidget
{
    Q_OBJECT

public:
    explicit TrackControlPanel(QWidget *parent = nullptr);
    ~TrackControlPanel();

signals:
    /**
     * @brief 请求加载文件信号
     * @param filePath 文件路径
     */
    void loadFileRequested(const QString &filePath);

    /**
     * @brief 播放请求信号
     */
    void playRequested();

    /**
     * @brief 暂停请求信号
     */
    void pauseRequested();

    /**
     * @brief 停止请求信号
     */
    void stopRequested();

    /**
     * @brief 速度变更信号
     * @param speed 速度倍率 (1, 2, 4)
     */
    void speedChanged(int speed);

public slots:
    /**
     * @brief 文件加载成功
     */
    void onFileLoaded();

    /**
     * @brief 文件加载失败
     * @param errorMsg 错误信息
     */
    void onFileLoadError(const QString &errorMsg);

    /**
     * @brief 回放开始
     */
    void onPlaybackStarted();

    /**
     * @brief 回放暂停
     */
    void onPlaybackPaused();

    /**
     * @brief 回放停止
     */
    void onPlaybackStopped();

    /**
     * @brief 回放完成
     */
    void onPlaybackFinished();

    /**
     * @brief 进度更新
     * @param current 当前时间
     * @param total 总时长
     */
    void onProgressChanged(double current, double total);

private slots:
    void onLoadFileClicked();
    void onPlayClicked();
    void onPauseClicked();
    void onStopClicked();
    void onSpeed1xToggled(bool checked);
    void onSpeed2xToggled(bool checked);
    void onSpeed4xToggled(bool checked);

private:
    void updateSpeedButtons(int speed);
    QString formatTime(double seconds);

private:
    Ui::TrackControlPanel *ui;
};

#endif // TRACKCONTROLPANEL_H
