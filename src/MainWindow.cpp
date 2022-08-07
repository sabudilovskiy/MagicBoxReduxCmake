#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "xml.h"
#include <string>
#include <sstream>
#include <magic_enum.hpp>
#include <codecvt>
#include <filesystem>
#include <fstream>
#include <QMessageBox>
#include <QMenu>
#include <QDebug>
namespace fs = std::filesystem;

MainWindow::MainWindow(AppSettings &app_settings, Downloader &downloader, QWidget *parent) :
        QMainWindow(parent),
        downloader(downloader),
        app_settings(app_settings),
        mod_downloader(app_settings.get_setting(L"url_config_ModDownloader")->value,
                       app_settings.get_setting(L"path_config_ModDownloader")->value,
                       downloader),
        mod_controller(app_settings.get_setting(L"path_game")->value,
                       app_settings.get_setting(L"path_config_ModController")->value),
        ui(new Ui::MainWindow)
{

}

MainWindow::~MainWindow()
{
    delete ui;
}

