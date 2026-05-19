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

private slots:
    void onLoadTrackRequested();
    void onPlayRequested();
    void onPauseRequested();
    void onStopTrackRequested();
    void onSpeedChanged(int multiplier);
    void onPlaybackFinished();

private:
    /**
     * @brief 初始化UI样式
     */
    void setupStyle();

    /**
     * @brief 连接信号槽
     */
    void connectSignals();

private:
    Ui::MainWindow *ui;
    RadarWidget *m_radarWidget;
    TrackPlayer *m_trackPlayer;
    TrackControlPanel *m_trackControlPanel;
    bool m_isRunning;
};

#endif // MAINWINDOW_H
