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
{
    ui->setupUi(this);

    connect(ui->loadFileButton, &QPushButton::clicked, this, &TrackControlPanel::onLoadFileClicked);
    connect(ui->playButton, &QPushButton::clicked, this, &TrackControlPanel::onPlayClicked);
    connect(ui->pauseButton, &QPushButton::clicked, this, &TrackControlPanel::onPauseClicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &TrackControlPanel::onStopClicked);

    connect(ui->speed1xButton, &QPushButton::toggled, this, &TrackControlPanel::onSpeed1xToggled);
    connect(ui->speed2xButton, &QPushButton::toggled, this, &TrackControlPanel::onSpeed2xToggled);
    connect(ui->speed4xButton, &QPushButton::toggled, this, &TrackControlPanel::onSpeed4xToggled);
}

TrackControlPanel::~TrackControlPanel()
{
    delete ui;
}

void TrackControlPanel::onLoadFileClicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        QStringLiteral("选择轨迹数据文件"),
        QString(),
        QStringLiteral("JSON文件 (*.json)")
    );

    if (!filePath.isEmpty()) {
        emit loadFileRequested(filePath);
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

void TrackControlPanel::onSpeed1xToggled(bool checked)
{
    if (checked) {
        ui->speed2xButton->setChecked(false);
        ui->speed4xButton->setChecked(false);
        emit speedChanged(1);
    }
}

void TrackControlPanel::onSpeed2xToggled(bool checked)
{
    if (checked) {
        ui->speed1xButton->setChecked(false);
        ui->speed4xButton->setChecked(false);
        emit speedChanged(2);
    }
}

void TrackControlPanel::onSpeed4xToggled(bool checked)
{
    if (checked) {
        ui->speed1xButton->setChecked(false);
        ui->speed2xButton->setChecked(false);
        emit speedChanged(4);
    }
}

void TrackControlPanel::onFileLoaded()
{
    ui->playButton->setEnabled(true);
    ui->stopButton->setEnabled(true);
    ui->speed1xButton->setEnabled(true);
    ui->speed2xButton->setEnabled(true);
    ui->speed4xButton->setEnabled(true);
}

void TrackControlPanel::onFileLoadError(const QString &errorMsg)
{
    QMessageBox::warning(this, QStringLiteral("加载失败"), errorMsg);
}

void TrackControlPanel::onPlaybackStarted()
{
    ui->playButton->setEnabled(false);
    ui->pauseButton->setEnabled(true);
}

void TrackControlPanel::onPlaybackPaused()
{
    ui->playButton->setEnabled(true);
    ui->pauseButton->setEnabled(false);
}

void TrackControlPanel::onPlaybackStopped()
{
    ui->playButton->setEnabled(true);
    ui->pauseButton->setEnabled(false);
    ui->progressLabel->setText(formatTime(0.0) + " / " + formatTime(0.0));
}

void TrackControlPanel::onPlaybackFinished()
{
    ui->playButton->setEnabled(true);
    ui->pauseButton->setEnabled(false);
}

void TrackControlPanel::onProgressChanged(double current, double total)
{
    ui->progressLabel->setText(formatTime(current) + " / " + formatTime(total));
}

void TrackControlPanel::updateSpeedButtons(int speed)
{
    ui->speed1xButton->setChecked(speed == 1);
    ui->speed2xButton->setChecked(speed == 2);
    ui->speed4xButton->setChecked(speed == 4);
}

QString TrackControlPanel::formatTime(double seconds)
{
    int totalSeconds = static_cast<int>(seconds);
    int mins = totalSeconds / 60;
    int secs = totalSeconds % 60;
    return QString("%1:%2").arg(mins, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'));
}
