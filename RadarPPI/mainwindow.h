/**
 * @file mainwindow.h
 * @brief 主窗口类声明
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// 前向声明
class RadarWidget;
class TrackPlayer;
class TrackControlPanel;

namespace Ui {
class MainWindow;
}

/**
 * @class MainWindow
 * @brief 雷达PPI显示系统主窗口
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    /**
     * @brief 启动雷达扫描
     */
    void onStartClicked();

    /**
     * @brief 停止雷达扫描
     */
    void onStopClicked();

    /**
     * @brief 重置雷达状态
     */
    void onResetClicked();

    /**
     * @brief 更新状态栏显示
     * @param angle 当前扫描角度
     */
    void updateStatusBar(double angle);

    /**
     * @brief 轨迹加载完成处理
     * @param success 是否加载成功
     * @param message 加载消息
     */
    void onTrackLoaded(bool success, const QString &message);

    /**
     * @brief 回放状态变化处理
     * @param isPlaying 是否正在播放
     */
    void onTrackPlayStateChanged(bool isPlaying);

private:
    /**
     * @brief 初始化UI样式
     */
    void setupStyle();

    /**
     * @brief 连接信号槽
     */
    void connectSignals();

    /**
     * @brief 连接轨迹回放相关信号槽
     */
    void connectTrackSignals();

private:
    Ui::MainWindow *ui;
    RadarWidget *m_radarWidget;         ///< 雷达显示控件
    TrackPlayer *m_trackPlayer;         ///< 轨迹回放控制器
    TrackControlPanel *m_trackPanel;    ///< 轨迹回放控制面板
    bool m_isRunning;                   ///< 运行状态
    bool m_isTrackPlaying;              ///< 轨迹回放状态
};

#endif // MAINWINDOW_H
