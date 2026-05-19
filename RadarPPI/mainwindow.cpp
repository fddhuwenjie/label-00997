/**
 * @file mainwindow.cpp
 * @brief 主窗口类实现
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "radarwidget.h"
#include "trackplayer.h"
#include "trackcontrolpanel.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_radarWidget(nullptr)
    , m_trackPlayer(nullptr)
    , m_trackPanel(nullptr)
    , m_isRunning(false)
    , m_isTrackPlaying(false)
{
    ui->setupUi(this);

    setWindowTitle("RadarPPI - 雷达P型显示仿真系统");
    setMinimumSize(500, 670);
    resize(700, 870);

    m_radarWidget = new RadarWidget(this);
    ui->radarContainer->layout()->addWidget(m_radarWidget);

    m_trackPlayer = new TrackPlayer(this);
    m_trackPanel = new TrackControlPanel(this);

    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout *>(ui->centralwidget->layout());
    if (mainLayout) {
        int controlPanelIndex = mainLayout->indexOf(ui->controlPanel);
        if (controlPanelIndex >= 0) {
            mainLayout->insertWidget(controlPanelIndex, m_trackPanel);
        }
    }

    setupStyle();
    connectSignals();
    connectTrackSignals();
    updateStatusBar(0.0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupStyle()
{
    // 全局深色主题样式
    QString globalStyle = R"(
        QMainWindow {
            background-color: #0A0F0A;
        }
        QWidget#centralwidget {
            background-color: #0A0F0A;
        }
        QWidget#radarContainer {
            background-color: #0A0F0A;
            border: none;
        }
        QWidget#controlPanel {
            background-color: #0F1410;
            border-top: 1px solid #1A251A;
        }
        QPushButton {
            background-color: #1A251A;
            border: 1px solid #007A1E;
            border-radius: 4px;
            color: #00AA2A;
            padding: 8px 20px;
            min-width: 80px;
            font-size: 13px;
        }
        QPushButton:hover {
            background-color: #243024;
            border-color: #00AA2A;
            color: #00FF41;
        }
        QPushButton:pressed {
            background-color: #00AA2A;
            color: #0A0F0A;
        }
        QPushButton:disabled {
            background-color: #0F1410;
            border-color: #1A251A;
            color: #4A5A4A;
        }
        QPushButton#startButton {
            border-color: #00FF41;
            color: #00FF41;
        }
        QPushButton#stopButton {
            border-color: #FF3333;
            color: #FF3333;
        }
        QPushButton#stopButton:hover {
            background-color: #3A1A1A;
        }
        QLabel {
            color: #00AA2A;
            font-size: 12px;
        }
        QLabel#statusLabel {
            color: #00AA2A;
            font-family: Consolas, Monaco, monospace;
        }
        QStatusBar {
            background-color: #0F1410;
            border-top: 1px solid #1A251A;
            color: #00AA2A;
            font-size: 12px;
        }
    )";

    this->setStyleSheet(globalStyle);
}

void MainWindow::connectSignals()
{
    // 按钮信号连接
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onStartClicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::onStopClicked);
    connect(ui->resetButton, &QPushButton::clicked, this, &MainWindow::onResetClicked);

    // 雷达控件信号连接
    connect(m_radarWidget, &RadarWidget::angleChanged, this, &MainWindow::updateStatusBar);

    // 初始按钮状态
    ui->stopButton->setEnabled(false);
}

void MainWindow::onStartClicked()
{
    if (!m_isRunning) {
        m_isRunning = true;
        m_radarWidget->startScan();
        ui->startButton->setEnabled(false);
        ui->stopButton->setEnabled(true);
        ui->statusLabel->setText("状态: 运行中");
    }
}

void MainWindow::updateStatusBar(double angle)
{
    QString angleText = QString("方位: %1°").arg(angle, 5, 'f', 1, ' ');
    ui->angleLabel->setText(angleText);

    QString rpmText = QString("转速: %1 RPM").arg(m_radarWidget->getRPM());
    ui->rpmLabel->setText(rpmText);
}

void MainWindow::connectTrackSignals()
{
    connect(m_trackPanel, &TrackControlPanel::loadTrackRequested,
            m_trackPlayer, &TrackPlayer::loadFromFile);
    connect(m_trackPanel, &TrackControlPanel::playRequested,
            m_trackPlayer, &TrackPlayer::play);
    connect(m_trackPanel, &TrackControlPanel::pauseRequested,
            m_trackPlayer, &TrackPlayer::pause);
    connect(m_trackPanel, &TrackControlPanel::stopRequested,
            m_trackPlayer, &TrackPlayer::stop);
    connect(m_trackPanel, &TrackControlPanel::speedRequested,
            m_trackPlayer, &TrackPlayer::setSpeed);
    connect(m_trackPanel, &TrackControlPanel::seekRequested,
            m_trackPlayer, &TrackPlayer::seek);

    connect(m_trackPlayer, &TrackPlayer::trackLoaded,
            m_trackPanel, &TrackControlPanel::onTrackLoaded);
    connect(m_trackPlayer, &TrackPlayer::trackLoaded,
            this, &MainWindow::onTrackLoaded);
    connect(m_trackPlayer, &TrackPlayer::playStateChanged,
            m_trackPanel, &TrackControlPanel::onPlayStateChanged);
    connect(m_trackPlayer, &TrackPlayer::playStateChanged,
            this, &MainWindow::onTrackPlayStateChanged);
    connect(m_trackPlayer, &TrackPlayer::timeChanged,
            m_trackPanel, &TrackControlPanel::onTimeChanged);
    connect(m_trackPlayer, &TrackPlayer::speedChanged,
            m_trackPanel, &TrackControlPanel::onSpeedChanged);
    connect(m_trackPlayer, &TrackPlayer::playbackFinished,
            m_trackPanel, &TrackControlPanel::onPlaybackFinished);
    connect(m_trackPlayer, &TrackPlayer::targetsUpdated,
            m_radarWidget, &RadarWidget::onTrackTargetsUpdated);
}

void MainWindow::onTrackLoaded(bool success, const QString &message)
{
    Q_UNUSED(message);
    if (success) {
        m_radarWidget->setTrackPlaybackMode(true);
        if (m_isRunning) {
            onStopClicked();
        }
        ui->startButton->setEnabled(false);
        ui->statusLabel->setText("状态: 轨迹回放模式");
    } else {
        m_radarWidget->setTrackPlaybackMode(false);
        ui->startButton->setEnabled(true);
        ui->statusLabel->setText("状态: 就绪");
    }
}

void MainWindow::onTrackPlayStateChanged(bool isPlaying)
{
    m_isTrackPlaying = isPlaying;
    if (isPlaying) {
        ui->statusLabel->setText("状态: 轨迹回放中");
    } else {
        ui->statusLabel->setText("状态: 轨迹回放已暂停");
    }
}

void MainWindow::onStopClicked()
{
    if (m_isRunning) {
        m_isRunning = false;
        m_radarWidget->stopScan();
        ui->startButton->setEnabled(true);
        ui->stopButton->setEnabled(false);
        ui->statusLabel->setText("状态: 已停止");
    }

    if (m_trackPlayer->isLoaded()) {
        m_trackPlayer->stop();
    }
}

void MainWindow::onResetClicked()
{
    m_isRunning = false;
    m_isTrackPlaying = false;
    m_radarWidget->reset();
    m_radarWidget->setTrackPlaybackMode(false);
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    ui->statusLabel->setText("状态: 已重置");
    updateStatusBar(0.0);

    if (m_trackPlayer->isLoaded()) {
        m_trackPlayer->stop();
    }
}
