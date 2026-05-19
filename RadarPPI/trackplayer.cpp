/**
 * @file trackplayer.cpp
 * @brief 目标轨迹回放播放器类实现
 */

#include "trackplayer.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QDebug>

TrackPlayer::TrackPlayer(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
    , m_currentIndex(0)
    , m_currentTime(0.0)
    , m_totalDuration(0.0)
    , m_isPlaying(false)
    , m_speed(Speed1x)
{
    connect(m_timer, &QTimer::timeout, this, &TrackPlayer::onTimerUpdate);
}

TrackPlayer::~TrackPlayer()
{
}

bool TrackPlayer::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit errorOccurred(QString("无法打开文件: %1").arg(filePath));
        return false;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    if (!parseJson(jsonData)) {
        return false;
    }

    m_totalDuration = 0.0;
    if (m_trackPoints.size() > 0) {
        m_totalDuration = m_trackPoints.last().timestamp;
    }

    return true;
}

bool TrackPlayer::parseJson(const QByteArray &jsonData)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        emit errorOccurred(QString("JSON解析错误: %1").arg(parseError.errorString()));
        return false;
    }

    if (!doc.isObject()) {
        emit errorOccurred("JSON数据格式错误: 需要根对象");
        return false;
    }

    QJsonObject root = doc.object();
    if (!root.contains("tracks")) {
        emit errorOccurred("JSON数据缺少tracks字段");
        return false;
    }

    QJsonArray tracksArray = root["tracks"].toArray();
    m_trackPoints.clear();

    for (const QJsonValue &trackValue : tracksArray) {
        if (!trackValue.isObject()) continue;

        QJsonObject track = trackValue.toObject();

        TrackPoint point;
        point.timestamp = track.value("timestamp").toDouble(0.0);
        point.azimuth = track.value("azimuth").toDouble(0.0);
        point.distance = track.value("distance").toDouble(0.0);
        point.targetId = track.value("targetId").toInt(0);

        m_trackPoints.append(point);
    }

    std::sort(m_trackPoints.begin(), m_trackPoints.end(),
              [](const TrackPoint &a, const TrackPoint &b) {
                  return a.timestamp < b.timestamp;
              });

    if (m_trackPoints.isEmpty()) {
        emit errorOccurred("轨迹数据为空");
        return false;
    }

    return true;
}

void TrackPlayer::play()
{
    if (m_trackPoints.isEmpty()) {
        emit errorOccurred("没有可回放的轨迹数据");
        return;
    }

    if (m_isPlaying) {
        return;
    }

    m_isPlaying = true;
    emit playbackStarted();
    m_timer->start(16);
}

void TrackPlayer::pause()
{
    if (!m_isPlaying) {
        return;
    }

    m_isPlaying = false;
    m_timer->stop();
    emit playbackPaused();
}

void TrackPlayer::stop()
{
    m_isPlaying = false;
    m_timer->stop();
    m_currentIndex = 0;
    m_currentTime = 0.0;
    emit playbackStopped();
    emit progressChanged(m_currentTime, m_totalDuration);
}

void TrackPlayer::setPlaybackSpeed(PlaybackSpeed speed)
{
    m_speed = speed;
}

void TrackPlayer::onTimerUpdate()
{
    double deltaTime = 0.016 * m_speed;
    m_currentTime += deltaTime;

    if (m_currentTime >= m_totalDuration) {
        m_currentTime = m_totalDuration;
        emitUpdatesUpToCurrentTime();
        m_timer->stop();
        m_isPlaying = false;
        emit progressChanged(m_currentTime, m_totalDuration);
        emit playbackFinished();
        return;
    }

    emitUpdatesUpToCurrentTime();
    emit progressChanged(m_currentTime, m_totalDuration);
}

void TrackPlayer::emitUpdatesUpToCurrentTime()
{
    while (m_currentIndex < m_trackPoints.size() &&
           m_trackPoints[m_currentIndex].timestamp <= m_currentTime) {
        const TrackPoint &point = m_trackPoints[m_currentIndex];
        emit targetUpdated(point.targetId, point.azimuth, point.distance);
        ++m_currentIndex;
    }
}
