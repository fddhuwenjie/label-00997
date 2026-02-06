/**
 * @file main.cpp
 * @brief RadarPPI - 雷达P型显示仿真系统入口
 * @version 1.0
 */

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    // 启用高DPI支持
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);

    // 设置应用程序信息
    app.setApplicationName("RadarPPI");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("Alkaid");

    // 创建并显示主窗口
    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
