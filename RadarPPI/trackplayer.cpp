/**
 * @file trackplayer.cpp
 * @brief 目标轨迹回放控制器类实现
 */

#include "trackplayer.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QDateTime>
#include <QDebug>

TrackPlayer::TrackPlayer(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
    , m_isLoaded(false)
    , m_isPlaying(false)
    , m_speed(1.0)
    , m_currentTime(0)
    , m_totalDuration(0)
    , m_lastUpdateTime(0)
{
    m_timer->setInterval(50); // 20 FPS 回放更新
    connect(m_timer, &QTimer::timeout, this, &TrackPlayer::onTimerUpdate);
}

TrackPlayer::~TrackPlayer()
{
    if (m_timer->isActive()) {
        m_timer->stop();
    }
}

bool TrackPlayer::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit trackLoaded(false, QString("无法打开文件: %1").arg(filePath));
        return false;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        emit trackLoaded(false, QString("JSON解析错误: %1").arg(parseError.errorString()));
        return false;
    }

    if (!doc.isObject()) {
        emit trackLoaded(false, "JSON 根节点必须是对象");
        return false;
    }

    QJsonObject root = doc.object();

    if (!root.contains("tracks") || !root["tracks"].isArray()) {
        emit trackLoaded(false, "JSON 必须包含 'tracks' 数组");
        return false;
    }

    m_trackData.clear();
    m_targetIds.clear();
    m_totalDuration = 0;

    QJsonArray tracksArray = root["tracks"].toArray();
    for (const QJsonValue &trackValue : tracksArray) {
        if (!trackValue.isObject()) continue;

        QJsonObject trackObj = trackValue.toObject();
        if (!trackObj.contains("targetId") || !trackObj.contains("points")) {
            continue;
        }

        QString targetId = trackObj["targetId"].toString();
        QJsonArray pointsArray = trackObj["points"].toArray();

        QVector<TrackPoint> points;
        for (const QJsonValue &pointValue : pointsArray) {
            if (!pointValue.isObject()) continue;

            QJsonObject pointObj = pointValue.toObject();
            if (!pointObj.contains("timestamp") ||
                !pointObj.contains("azimuth") ||
                !pointObj.contains("distance")) {
                continue;
            }

            qint64 timestamp = pointObj["timestamp"].toVariant().toLongLong();
            double azimuth = pointObj["azimuth"].toDouble();
            double distance = pointObj["distance"].toDouble();

            distance = qBound(0.0, distance, 1.0);
            while (azimuth < 0.0) azimuth += 360.0;
            while (azimuth >= 360.0) azimuth -= 360.0;

            points.append(TrackPoint(timestamp, azimuth, distance, targetId));

            if (timestamp > m_totalDuration) {
                m_totalDuration = timestamp;
            }
        }

        if (!points.isEmpty()) {
            std::sort(points.begin(), points.end(),
                      [](const TrackPoint &a, const TrackPoint &b) {
                          return a.timestamp < b.timestamp;
                      });
            m_trackData[targetId] = points;
            m_targetIds.append(targetId);
        }
    }

    if (m_targetIds.isEmpty()) {
        emit trackLoaded(false, "未找到有效的轨迹数据");
        return false;
    }

    m_isLoaded = true;
    m_currentTime = 0;
    m_activeTargets.clear();

    QString message = QString("轨迹加载成功: %1 个目标, 总时长 %2 秒")
                          .arg(m_targetIds.size())
                          .arg(m_totalDuration / 1000.0, 0, 'f', 1);

    emit trackLoaded(true, message);
    emit timeChanged(m_currentTime, m_totalDuration);

    return true;
}

void TrackPlayer::play()
{
    if (!m_isLoaded || m_isPlaying) return;

    m_isPlaying = true;
    m_lastUpdateTime = QDateTime::currentMSecsSinceEpoch();
    m_timer->start();

    emit playStateChanged(m_isPlaying);
}

void TrackPlayer::pause()
{
    if (!m_isPlaying) return;

    m_isPlaying = false;
    m_timer->stop();

    emit playStateChanged(m_isPlaying);
}

void TrackPlayer::stop()
{
    m_isPlaying = false;
    m_timer->stop();
    m_currentTime = 0;
    m_activeTargets.clear();

    emit playStateChanged(m_isPlaying);
    emit timeChanged(m_currentTime, m_totalDuration);
    emit targetsUpdated(m_activeTargets);
}

void TrackPlayer::setSpeed(double speed)
{
    m_speed = qBound(1.0, speed, 4.0);
    emit speedChanged(m_speed);
}

void TrackPlayer::seek(qint64 time)
{
    if (!m_isLoaded) return;

    m_currentTime = qBound(0LL, time, m_totalDuration);
    updateTargetsAtCurrentTime();

    emit timeChanged(m_currentTime, m_totalDuration);
    emit targetsUpdated(m_activeTargets);
}

void TrackPlayer::onTimerUpdate()
{
    if (!m_isPlaying || !m_isLoaded) return;

    qint64 now = QDateTime::currentMSecsSinceEpoch();
    qint64 delta = (now - m_lastUpdateTime) * m_speed;
    m_lastUpdateTime = now;

    m_currentTime += delta;

    if (m_currentTime >= m_totalDuration) {
        m_currentTime = m_totalDuration;
        m_isPlaying = false;
        m_timer->stop();

        emit playStateChanged(m_isPlaying);
        emit playbackFinished();
    }

    updateTargetsAtCurrentTime();

    emit timeChanged(m_currentTime, m_totalDuration);
    emit targetsUpdated(m_activeTargets);
}

void TrackPlayer::updateTargetsAtCurrentTime()
{
    m_activeTargets.clear();

    for (const QString &targetId : m_targetIds) {
        double azimuth = 0.0;
        double distance = 0.0;

        if (interpolateTargetPosition(targetId, m_currentTime, azimuth, distance)) {
            m_activeTargets.append(ActiveTrackTarget(targetId, azimuth, distance));
        }
    }
}

bool TrackPlayer::interpolateTargetPosition(const QString &targetId, qint64 time,
                                            double &outAzimuth, double &outDistance)
{
    if (!m_trackData.contains(targetId)) return false;

    const QVector<TrackPoint> &points = m_trackData[targetId];
    if (points.isEmpty()) return false;

    if (time < points.first().timestamp) return false;
    if (time > points.last().timestamp) return false;

    int left = 0;
    int right = points.size() - 1;

    while (left <= right) {
        int mid = (left + right) / 2;
        if (points[mid].timestamp == time) {
            outAzimuth = points[mid].azimuth;
            outDistance = points[mid].distance;
            return true;
        } else if (points[mid].timestamp < time) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    int index1 = qMax(0, right);
    int index2 = qMin(points.size() - 1, left);

    if (index1 == index2) {
        outAzimuth = points[index1].azimuth;
        outDistance = points[index1].distance;
        return true;
    }

    const TrackPoint &p1 = points[index1];
    const TrackPoint &p2 = points[index2];

    qint64 timeSpan = p2.timestamp - p1.timestamp;
    if (timeSpan == 0) {
        outAzimuth = p1.azimuth;
        outDistance = p1.distance;
        return true;
    }

    double t = double(time - p1.timestamp) / double(timeSpan);

    double angleDiff = p2.azimuth - p1.azimuth;
    if (angleDiff > 180.0) angleDiff -= 360.0;
    if (angleDiff < -180.0) angleDiff += 360.0;

    outAzimuth = p1.azimuth + angleDiff * t;
    while (outAzimuth < 0.0) outAzimuth += 360.0;
    while (outAzimuth >= 360.0) outAzimuth -= 360.0;

    outDistance = p1.distance + (p2.distance - p1.distance) * t;
    outDistance = qBound(0.0, outDistance, 1.0);

    return true;
}
