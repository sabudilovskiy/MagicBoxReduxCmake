#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidgetItem>
#include <QMainWindow>
#include <vector>
#include <unordered_map>
#include "Downloader.h"
#include "ModController.h"
#include "ModDownloader.h"
#include "AppSettings.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
protected:
    Q_OBJECT
    Downloader& downloader;
    AppSettings& app_settings;
    Ui::MainWindow *ui;
    ModController mod_controller;
    ModDownloader mod_downloader;
    void run_game();
public:
    MainWindow(AppSettings& app_settings, Downloader& downloader, QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void reload_mods();
protected slots:
    void on_rightClicked_db(const QPoint&pos);
    void on_rightClicked_sdk(const QPoint&pos);
private slots:
    void on_pushButton_reinstall_clicked();
    void on_pushButton_start_clicked();
};
#endif // MAINWINDOW_H
