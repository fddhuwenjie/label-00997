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
    , m_isRunning(false)
{
    ui->setupUi(this);

    // 设置窗口属性
    setWindowTitle("RadarPPI - 雷达P型显示仿真系统");
    setMinimumSize(500, 550);
    resize(700, 750);

    // 创建雷达显示控件
    m_radarWidget = new RadarWidget(this);
    ui->radarContainer->layout()->addWidget(m_radarWidget);

    // 初始化轨迹回放模块
    initTrackPlayback();

    // 初始化样式
    setupStyle();

    // 连接信号槽
    connectSignals();

    // 更新初始状态
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
        QWidget#trackControlContainer {
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
        QPushButton:checked {
            background-color: #004A12;
            border-color: #00FF41;
            color: #00FF41;
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
        QLabel#progressLabel {
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

    // 轨迹回放信号槽连接
    connect(m_trackControlPanel, &TrackControlPanel::loadFileRequested,
            m_trackPlayer, &TrackPlayer::loadFromFile);
    connect(m_trackControlPanel, &TrackControlPanel::playRequested,
            m_trackPlayer, &TrackPlayer::play);
    connect(m_trackControlPanel, &TrackControlPanel::pauseRequested,
            m_trackPlayer, &TrackPlayer::pause);
    connect(m_trackControlPanel, &TrackControlPanel::stopRequested,
            m_trackPlayer, &TrackPlayer::stop);
    connect(m_trackControlPanel, &TrackControlPanel::speedChanged,
            m_trackPlayer, [this](int speed) {
                m_trackPlayer->setPlaybackSpeed(static_cast<TrackPlayer::PlaybackSpeed>(speed));
            });

    // TrackPlayer -> TrackControlPanel
    connect(m_trackPlayer, &TrackPlayer::playbackStarted,
            m_trackControlPanel, &TrackControlPanel::onPlaybackStarted);
    connect(m_trackPlayer, &TrackPlayer::playbackPaused,
            m_trackControlPanel, &TrackControlPanel::onPlaybackPaused);
    connect(m_trackPlayer, &TrackPlayer::playbackStopped,
            m_trackControlPanel, &TrackControlPanel::onPlaybackStopped);
    connect(m_trackPlayer, &TrackPlayer::playbackFinished,
            m_trackControlPanel, &TrackControlPanel::onPlaybackFinished);
    connect(m_trackPlayer, &TrackPlayer::progressChanged,
            m_trackControlPanel, &TrackControlPanel::onProgressChanged);
    connect(m_trackPlayer, &TrackPlayer::errorOccurred,
            m_trackControlPanel, &TrackControlPanel::onFileLoadError);

    // 初始按钮状态
    ui->stopButton->setEnabled(false);
}

void MainWindow::initTrackPlayback()
{
    // 创建轨迹回放播放器
    m_trackPlayer = new TrackPlayer(this);

    // 创建轨迹回放控制面板
    m_trackControlPanel = new TrackControlPanel(this);
    ui->trackControlContainer->layout()->addWidget(m_trackControlPanel);
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

void MainWindow::onStopClicked()
{
    if (m_isRunning) {
        m_isRunning = false;
        m_radarWidget->stopScan();
        ui->startButton->setEnabled(true);
        ui->stopButton->setEnabled(false);
        ui->statusLabel->setText("状态: 已停止");
    }
}

void MainWindow::onResetClicked()
{
    m_isRunning = false;
    m_radarWidget->reset();
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    ui->statusLabel->setText("状态: 已重置");
    updateStatusBar(0.0);
}

void MainWindow::updateStatusBar(double angle)
{
    QString angleText = QString("方位: %1°").arg(angle, 5, 'f', 1, ' ');
    ui->angleLabel->setText(angleText);

    QString rpmText = QString("转速: %1 RPM").arg(m_radarWidget->getRPM());
    ui->rpmLabel->setText(rpmText);
}
