/**
 * @file trackcontrolpanel.cpp
 * @brief 轨迹回放控制面板类实现
 */

#include "trackcontrolpanel.h"
#include "ui_trackcontrolpanel.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

TrackControlPanel::TrackControlPanel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TrackControlPanel)
    , m_totalDuration(0)
    , m_isPlaying(false)
    , m_isDraggingSlider(false)
{
    ui->setupUi(this);
    connectInternalSignals();
}

TrackControlPanel::~TrackControlPanel()
{
    delete ui;
}

void TrackControlPanel::connectInternalSignals()
{
    connect(ui->loadButton, &QPushButton::clicked, this, &TrackControlPanel::onLoadClicked);
    connect(ui->playButton, &QPushButton::clicked, this, &TrackControlPanel::onPlayClicked);
    connect(ui->pauseButton, &QPushButton::clicked, this, &TrackControlPanel::onPauseClicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &TrackControlPanel::onStopClicked);
    connect(ui->speedComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TrackControlPanel::onSpeedIndexChanged);
    connect(ui->progressSlider, &QSlider::valueChanged, this, &TrackControlPanel::onSliderValueChanged);
    connect(ui->progressSlider, &QSlider::sliderReleased, this, &TrackControlPanel::onSliderReleased);
}

void TrackControlPanel::onLoadClicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("选择轨迹文件"),
        QString(),
        tr("JSON 轨迹文件 (*.json);;所有文件 (*.*)")
    );

    if (!filePath.isEmpty()) {
        emit loadTrackRequested(filePath);
    }
}

void TrackControlPanel::onPlayClicked()
{
    emit playRequested();
}

void TrackControlPanel::onPauseClicked()
{
    emit pauseRequested();
}

void TrackControlPanel::onStopClicked()
{
    emit stopRequested();
}

void TrackControlPanel::onSpeedIndexChanged(int index)
{
    double speed = 1.0;
    switch (index) {
    case 0: speed = 1.0; break;
    case 1: speed = 2.0; break;
    case 2: speed = 4.0; break;
    default: speed = 1.0; break;
    }
    emit speedRequested(speed);
}

void TrackControlPanel::onSliderValueChanged(int value)
{
    if (m_totalDuration > 0) {
        qint64 time = qint64(value * m_totalDuration / 1000.0);
        ui->timeLabel->setText(formatTime(time));
    }
}

void TrackControlPanel::onSliderReleased()
{
    if (m_totalDuration > 0) {
        qint64 time = qint64(ui->progressSlider->value() * m_totalDuration / 1000.0);
        emit seekRequested(time);
    }
}

void TrackControlPanel::onTrackLoaded(bool success, const QString &message)
{
    if (success) {
        ui->trackInfoLabel->setText(message);
        ui->trackInfoLabel->setStyleSheet("color: #00FF41;");
        ui->playButton->setEnabled(true);
        ui->stopButton->setEnabled(true);
        ui->speedComboBox->setEnabled(true);
        ui->progressSlider->setEnabled(true);
    } else {
        ui->trackInfoLabel->setText(QString("轨迹加载失败: %1").arg(message));
        ui->trackInfoLabel->setStyleSheet("color: #FF3333;");
        ui->playButton->setEnabled(false);
        ui->pauseButton->setEnabled(false);
        ui->stopButton->setEnabled(false);
        ui->speedComboBox->setEnabled(false);
        ui->progressSlider->setEnabled(false);
        ui->progressSlider->setValue(0);
        ui->timeLabel->setText("00:00.0");
        ui->durationLabel->setText("00:00.0");
        m_totalDuration = 0;
    }
}

void TrackControlPanel::onPlayStateChanged(bool isPlaying)
{
    m_isPlaying = isPlaying;
    ui->playButton->setEnabled(!isPlaying);
    ui->pauseButton->setEnabled(isPlaying);
}

void TrackControlPanel::onTimeChanged(qint64 currentTime, qint64 totalDuration)
{
    m_totalDuration = totalDuration;
    ui->durationLabel->setText(formatTime(totalDuration));

    if (totalDuration > 0 && !ui->progressSlider->isSliderDown()) {
        int progress = int(currentTime * 1000.0 / totalDuration);
        ui->progressSlider->blockSignals(true);
        ui->progressSlider->setValue(progress);
        ui->progressSlider->blockSignals(false);
        ui->timeLabel->setText(formatTime(currentTime));
    }
}

void TrackControlPanel::onSpeedChanged(double speed)
{
    int index = 0;
    if (qFuzzyCompare(speed, 2.0)) {
        index = 1;
    } else if (qFuzzyCompare(speed, 4.0)) {
        index = 2;
    }
    ui->speedComboBox->blockSignals(true);
    ui->speedComboBox->setCurrentIndex(index);
    ui->speedComboBox->blockSignals(false);
}

void TrackControlPanel::onPlaybackFinished()
{
    ui->playButton->setEnabled(true);
    ui->pauseButton->setEnabled(false);
}

QString TrackControlPanel::formatTime(qint64 milliseconds) const
{
    qint64 totalSeconds = milliseconds / 1000;
    qint64 minutes = totalSeconds / 60;
    qint64 seconds = totalSeconds % 60;
    qint64 tenths = (milliseconds % 1000) / 100;

    return QString("%1:%2.%3")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'))
        .arg(tenths, 1, 10, QChar('0'));
}
