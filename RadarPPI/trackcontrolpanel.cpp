/**
 * @file trackcontrolpanel.cpp
 * @brief 轨迹回放控制面板类实现
 */

#include "trackcontrolpanel.h"
#include "ui_trackcontrolpanel.h"

#include <QFileDialog>
#include <QMessageBox>

TrackControlPanel::TrackControlPanel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TrackControlPanel)
    , m_isPlaying(false)
{
    ui->setupUi(this);

    connect(ui->loadTrackButton, &QPushButton::clicked, this, &TrackControlPanel::onLoadClicked);
    connect(ui->playButton, &QPushButton::clicked, this, &TrackControlPanel::onPlayClicked);
    connect(ui->pauseButton, &QPushButton::clicked, this, &TrackControlPanel::onPauseClicked);
    connect(ui->stopTrackButton, &QPushButton::clicked, this, &TrackControlPanel::onStopClicked);
    connect(ui->speedComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &TrackControlPanel::onSpeedChanged);

    ui->playButton->setEnabled(false);
    ui->pauseButton->setEnabled(false);
    ui->stopTrackButton->setEnabled(false);
    ui->speedComboBox->setEnabled(false);
}

TrackControlPanel::~TrackControlPanel()
{
    delete ui;
}

void TrackControlPanel::onLoadClicked()
{
    emit loadTrackRequested();
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

void TrackControlPanel::onSpeedChanged(int index)
{
    int speeds[] = {1, 2, 4};
    int mult = (index >= 0 && index < 3) ? speeds[index] : 1;
    emit speedChanged(mult);
}

void TrackControlPanel::updateTimeDisplay(double currentTime, double totalTime)
{
    ui->timeLabel->setText(formatTime(currentTime) + " / " + formatTime(totalTime));
}

void TrackControlPanel::onTrackLoaded(bool success, int pointCount)
{
    Q_UNUSED(pointCount);
    if (success) {
        ui->playButton->setEnabled(true);
        ui->stopTrackButton->setEnabled(true);
        ui->speedComboBox->setEnabled(true);
        ui->timeLabel->setText(formatTime(0.0) + " / " + formatTime(0.0));
    } else {
        ui->playButton->setEnabled(false);
        ui->pauseButton->setEnabled(false);
        ui->stopTrackButton->setEnabled(false);
        ui->speedComboBox->setEnabled(false);
    }
}

void TrackControlPanel::onPlaybackFinished()
{
    m_isPlaying = false;
    ui->playButton->setEnabled(true);
    ui->pauseButton->setEnabled(false);
}

void TrackControlPanel::setPlayingState(bool playing)
{
    m_isPlaying = playing;
    ui->playButton->setEnabled(!playing);
    ui->pauseButton->setEnabled(playing);
}

QString TrackControlPanel::formatTime(double seconds) const
{
    int totalSec = qMax(0, static_cast<int>(seconds));
    int min = totalSec / 60;
    int sec = totalSec % 60;
    return QString("%1:%2").arg(min, 2, 10, QChar('0')).arg(sec, 2, 10, QChar('0'));
}
