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
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_radarWidget(nullptr)
    , m_trackPlayer(nullptr)
    , m_trackControlPanel(nullptr)
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

    // 创建轨迹回放控制器
    m_trackPlayer = new TrackPlayer(this);

    // 创建轨迹回放控制面板
    m_trackControlPanel = new TrackControlPanel(this);
    ui->trackControlContainer->layout()->addWidget(m_trackControlPanel);

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
            border-bottom: 1px solid #1A251A;
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
        QComboBox {
            background-color: #1A251A;
            border: 1px solid #007A1E;
            border-radius: 4px;
            color: #00AA2A;
            padding: 4px 8px;
            min-width: 60px;
        }
        QComboBox:hover {
            border-color: #00AA2A;
        }
        QComboBox::drop-down {
            border: none;
            width: 20px;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 4px solid transparent;
            border-right: 4px solid transparent;
            border-top: 6px solid #00AA2A;
        }
        QComboBox QAbstractItemView {
            background-color: #0F1410;
            border: 1px solid #007A1E;
            color: #00AA2A;
            selection-background-color: #1A251A;
            selection-color: #00FF41;
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

    // 轨迹回放控制面板信号
    connect(m_trackControlPanel, &TrackControlPanel::loadTrackRequested, this, &MainWindow::onLoadTrackRequested);
    connect(m_trackControlPanel, &TrackControlPanel::playRequested, this, &MainWindow::onPlayRequested);
    connect(m_trackControlPanel, &TrackControlPanel::pauseRequested, this, &MainWindow::onPauseRequested);
    connect(m_trackControlPanel, &TrackControlPanel::stopRequested, this, &MainWindow::onStopTrackRequested);
    connect(m_trackControlPanel, &TrackControlPanel::speedChanged, this, &MainWindow::onSpeedChanged);

    // 轨迹回放控制器信号 -> 雷达显示控件
    connect(m_trackPlayer, &TrackPlayer::trackPointUpdated, m_radarWidget, &RadarWidget::onTrackPointUpdated);
    connect(m_trackPlayer, &TrackPlayer::playbackTimeChanged, m_trackControlPanel, &TrackControlPanel::updateTimeDisplay);
    connect(m_trackPlayer, &TrackPlayer::playbackFinished, m_trackControlPanel, &TrackControlPanel::onPlaybackFinished);
    connect(m_trackPlayer, &TrackPlayer::playbackFinished, this, &MainWindow::onPlaybackFinished);
    connect(m_trackPlayer, &TrackPlayer::trackLoaded, m_trackControlPanel, &TrackControlPanel::onTrackLoaded);

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

void MainWindow::onLoadTrackRequested()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, "选择轨迹数据文件", QString(),
        "JSON 文件 (*.json);;所有文件 (*)");

    if (filePath.isEmpty()) return;

    m_trackPlayer->stop();
    m_radarWidget->clearTrackData();

    bool ok = m_trackPlayer->loadTrackFile(filePath);
    if (!ok) {
        QMessageBox::warning(this, "加载失败", "无法解析轨迹数据文件，请检查文件格式。");
    }
}

void MainWindow::onPlayRequested()
{
    m_trackPlayer->play();
    m_trackControlPanel->setPlayingState(true);
}

void MainWindow::onPauseRequested()
{
    m_trackPlayer->pause();
    m_trackControlPanel->setPlayingState(false);
}

void MainWindow::onStopTrackRequested()
{
    m_trackPlayer->stop();
    m_radarWidget->clearTrackData();
    m_trackControlPanel->setPlayingState(false);
}

void MainWindow::onSpeedChanged(int multiplier)
{
    m_trackPlayer->setSpeed(multiplier);
}

void MainWindow::onPlaybackFinished()
{
    m_trackControlPanel->setPlayingState(false);
}
