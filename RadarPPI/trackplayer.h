/**
 * @file trackplayer.h
 * @brief 目标轨迹回放控制器类声明
 */

#ifndef TRACKPLAYER_H
#define TRACKPLAYER_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QString>
#include <QMap>

struct TrackPoint {
    double timestamp;
    double azimuth;
    double distance;
    QString targetId;
};

class TrackPlayer : public QObject
{
    Q_OBJECT

public:
    explicit TrackPlayer(QObject *parent = nullptr);
    ~TrackPlayer();

    bool loadTrackFile(const QString &filePath);
    bool isLoaded() const;
    bool isPlaying() const;
    double getCurrentTime() const;
    double getTotalDuration() const;
    int getSpeedMultiplier() const;

signals:
    void trackPointUpdated(const QString &targetId, double azimuth, double distance);
    void playbackFinished();
    void playbackTimeChanged(double currentTime, double totalTime);
    void trackLoaded(bool success, int pointCount);

public slots:
    void play();
    void pause();
    void stop();
    void setSpeed(int multiplier);
    void seekToTime(double seconds);

private slots:
    void onPlaybackTick();

private:
    void rebuildIndex();
    void emitPointsUpTo(double time);

    QTimer *m_timer;
    QVector<TrackPoint> m_allPoints;
    QMap<QString, QVector<int>> m_targetIndex;
    int m_currentIndex;
    double m_currentTime;
    double m_totalDuration;
    int m_speedMultiplier;
    bool m_isPlaying;
    bool m_isLoaded;
    qint64 m_lastTickMs;
};

#endif // TRACKPLAYER_H
