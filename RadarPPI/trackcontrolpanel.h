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

class TrackControlPanel : public QWidget
{
    Q_OBJECT

public:
    explicit TrackControlPanel(QWidget *parent = nullptr);
    ~TrackControlPanel();

signals:
    void loadTrackRequested();
    void playRequested();
    void pauseRequested();
    void stopRequested();
    void speedChanged(int multiplier);

public slots:
    void updateTimeDisplay(double currentTime, double totalTime);
    void onTrackLoaded(bool success, int pointCount);
    void onPlaybackFinished();
    void setPlayingState(bool playing);

private slots:
    void onLoadClicked();
    void onPlayClicked();
    void onPauseClicked();
    void onStopClicked();
    void onSpeedChanged(int index);

private:
    QString formatTime(double seconds) const;

    Ui::TrackControlPanel *ui;
    bool m_isPlaying;
};

#endif // TRACKCONTROLPANEL_H
