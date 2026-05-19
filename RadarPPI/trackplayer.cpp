/**
 * @file trackplayer.cpp
 * @brief 目标轨迹回放控制器类实现
 */

#include "trackplayer.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>

TrackPlayer::TrackPlayer(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
    , m_currentIndex(0)
    , m_currentTime(0.0)
    , m_totalDuration(0.0)
    , m_speedMultiplier(1)
    , m_isPlaying(false)
    , m_isLoaded(false)
    , m_lastTickMs(0)
{
    m_timer->setInterval(33);
    connect(m_timer, &QTimer::timeout, this, &TrackPlayer::onPlaybackTick);
}

TrackPlayer::~TrackPlayer()
{
}

bool TrackPlayer::loadTrackFile(const QString &filePath)
{
    stop();
    m_allPoints.clear();
    m_targetIndex.clear();
    m_currentIndex = 0;
    m_currentTime = 0.0;
    m_totalDuration = 0.0;
    m_isLoaded = false;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit trackLoaded(false, 0);
        return false;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    file.close();

    if (doc.isNull() || !doc.isArray()) {
        emit trackLoaded(false, 0);
        return false;
    }

    QJsonArray trackArray = doc.array();
    for (int i = 0; i < trackArray.size(); ++i) {
        QJsonObject obj = trackArray[i].toObject();
        TrackPoint pt;
        pt.timestamp = obj["timestamp"].toDouble(0.0);
        pt.azimuth = obj["azimuth"].toDouble(0.0);
        pt.distance = obj["distance"].toDouble(0.0);
        pt.targetId = obj["targetId"].toString("");
        m_allPoints.append(pt);
    }

    if (m_allPoints.isEmpty()) {
        emit trackLoaded(false, 0);
        return false;
    }

    std::sort(m_allPoints.begin(), m_allPoints.end(),
              [](const TrackPoint &a, const TrackPoint &b) {
                  return a.timestamp < b.timestamp;
              });

    rebuildIndex();

    m_totalDuration = m_allPoints.last().timestamp - m_allPoints.first().timestamp;
    m_isLoaded = true;

    emit trackLoaded(true, m_allPoints.size());
    emit playbackTimeChanged(0.0, m_totalDuration);
    return true;
}

bool TrackPlayer::isLoaded() const
{
    return m_isLoaded;
}

bool TrackPlayer::isPlaying() const
{
    return m_isPlaying;
}

double TrackPlayer::getCurrentTime() const
{
    return m_currentTime;
}

double TrackPlayer::getTotalDuration() const
{
    return m_totalDuration;
}

int TrackPlayer::getSpeedMultiplier() const
{
    return m_speedMultiplier;
}

void TrackPlayer::play()
{
    if (!m_isLoaded) return;

    if (m_currentTime >= m_totalDuration) {
        m_currentTime = 0.0;
        m_currentIndex = 0;
    }

    m_isPlaying = true;
    m_lastTickMs = QDateTime::currentMSecsSinceEpoch();
    m_timer->start();
}

void TrackPlayer::pause()
{
    m_isPlaying = false;
    m_timer->stop();
}

void TrackPlayer::stop()
{
    m_isPlaying = false;
    m_timer->stop();
    m_currentTime = 0.0;
    m_currentIndex = 0;
    emit playbackTimeChanged(0.0, m_totalDuration);
}

void TrackPlayer::setSpeed(int multiplier)
{
    m_speedMultiplier = multiplier;
}

void TrackPlayer::seekToTime(double seconds)
{
    if (!m_isLoaded) return;
    m_currentTime = qBound(0.0, seconds, m_totalDuration);

    m_currentIndex = 0;
    for (int i = 0; i < m_allPoints.size(); ++i) {
        if (m_allPoints[i].timestamp - m_allPoints.first().timestamp > m_currentTime) {
            break;
        }
        m_currentIndex = i + 1;
    }

    emit playbackTimeChanged(m_currentTime, m_totalDuration);
}

void TrackPlayer::onPlaybackTick()
{
    if (!m_isPlaying || !m_isLoaded) return;

    qint64 nowMs = QDateTime::currentMSecsSinceEpoch();
    double deltaSec = (nowMs - m_lastTickMs) / 1000.0 * m_speedMultiplier;
    m_lastTickMs = nowMs;

    double prevTime = m_currentTime;
    m_currentTime += deltaSec;

    if (m_currentTime >= m_totalDuration) {
        m_currentTime = m_totalDuration;
        emitPointsUpTo(m_totalDuration);
        m_isPlaying = false;
        m_timer->stop();
        emit playbackTimeChanged(m_totalDuration, m_totalDuration);
        emit playbackFinished();
        return;
    }

    emitPointsUpTo(m_currentTime);
    emit playbackTimeChanged(m_currentTime, m_totalDuration);

    Q_UNUSED(prevTime);
}

void TrackPlayer::rebuildIndex()
{
    m_targetIndex.clear();
    for (int i = 0; i < m_allPoints.size(); ++i) {
        m_targetIndex[m_allPoints[i].targetId].append(i);
    }
}

void TrackPlayer::emitPointsUpTo(double time)
{
    if (m_allPoints.isEmpty()) return;

    double baseTime = m_allPoints.first().timestamp;

    while (m_currentIndex < m_allPoints.size()) {
        const TrackPoint &pt = m_allPoints[m_currentIndex];
        double relativeTime = pt.timestamp - baseTime;
        if (relativeTime > time) break;
        emit trackPointUpdated(pt.targetId, pt.azimuth, pt.distance);
        m_currentIndex++;
    }
}
