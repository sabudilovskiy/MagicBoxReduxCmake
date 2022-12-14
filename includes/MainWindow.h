#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <vector>
#include <unordered_map>

#include <QListWidgetItem>
#include <QMainWindow>
#include <QCloseEvent>

#include "Downloader.h"
#include "ModController.h"
#include "Mod.h"
#include "AppSettings.h"
#include "ModDownloader.h"
#include "DownloaderWindow.h"
#include "SettingsWindow.h"

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
    std::vector<ModVersion> installed_mods_db, installed_mods_sdk;
    std::unique_ptr<DownloaderWindow> manager_window;
    SettingsWindow* _settings_window = nullptr;
    void run_game();
    std::pair<int, int>  get_set();
    void close_settings_window();
public:
    MainWindow(AppSettings& app_settings, Downloader& downloader, QWidget *parent = nullptr);
    ~MainWindow();
    signals:
    void exit_app();
public slots:
    void reload_mods();
    void reload_db();
    void reload_sdk();
protected slots:
    void on_rightClicked_db(const QPoint&pos);
    void on_rightClicked_sdk(const QPoint&pos);
    void on_toolButton_set_delete_clicked();
    void on_toolButton_set_reinstall_clicked();
    void on_toolButton_set_config_clicked();
    void on_pushButton_settings_clicked();
    void on_pushButton_start_game_clicked();
    void on_pushButton_start_server_clicked();
    void on_MainWindow_destroyed();
    void on_toolButton_download_clicked();
    void closeEvent(QCloseEvent *event);
};
#endif // MAINWINDOW_H
