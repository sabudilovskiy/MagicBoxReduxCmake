#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "xml.h"
#include <filesystem>
#include <fstream>
#include <QMessageBox>
#include <QMenu>
#include <QProcess>
#include <QStringList>
#include "SettingsWindow.h"
namespace fs = std::filesystem;

MainWindow::MainWindow(AppSettings &app_settings, Downloader &downloader, QWidget *parent) :
        QMainWindow(parent),
        downloader(downloader),
        app_settings(app_settings),
        ui(new Ui::MainWindow),
        mod_controller(app_settings.get_setting(L"path_game"),
                       app_settings.get_setting(L"path_config_ModController")),
        mod_downloader(app_settings.get_setting(L"url_config_ModDownloader"),
                       app_settings.get_setting(L"path_config_ModDownloader"),
                       downloader, mod_controller)
{
    this->manager_window = std::make_unique<ManagerWindow>(app_settings, mod_downloader);
    ui->setupUi(this);
    reload_mods();
    QObject::connect(&mod_controller, &ModController::change_db, this, &MainWindow::reload_db);
    QObject::connect(&mod_controller, &ModController::change_sdk, this, &MainWindow::reload_sdk);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_rightClicked_db(const QPoint &pos) {

}

void MainWindow::on_rightClicked_sdk(const QPoint &pos) {

}

void MainWindow::on_pushButton_reinstall_clicked() {

}

void MainWindow::on_pushButton_start_clicked() {
    auto _db = ui->db_listWidget->selectedItems();
    auto _sdk = ui->sdk_listWidget->selectedItems();
    if (_db.size() != 1 && _sdk.size() != 1){
        auto* message_box = new QMessageBox();
        if (_db.empty()){
            message_box->critical(this, "Ошибка", "Вы не выбрали DB мод");
        }
        else if (_sdk.size() == 0){
            message_box->critical(this, "Ошибка", "Вы не выбрали SDK мод");
        }
        else if (_db.size() > 1){
            message_box->critical(this, "Ошибка", "Вы выбрали больше одного DB мода");
        }
        else {
            message_box->critical(this, "Ошибка", "Вы не выбрали DB мод");
        }
        message_box->exec();
        return;
    }
    int db_id =  ui->db_listWidget->row(_db[0]);
    int sdk_id = ui->sdk_listWidget->row(_sdk[0]);
    if (!mod_controller.exists_mods(installed_mods_db[db_id], installed_mods_sdk[sdk_id])){
        mod_controller.install_mods(installed_mods_db[db_id], installed_mods_sdk[sdk_id]);
    }
    mod_controller.update_mod(installed_mods_db[db_id], installed_mods_sdk[sdk_id]);
    QString path_game = QString::fromStdWString(app_settings.get_setting(L"path_game"));
    QString path_exe = path_game + "\\joshua.exe";
    QProcess::startDetached(path_exe,{}, path_game);
}


void MainWindow::on_pushButton_server_clicked()
{
    auto _db = ui->db_listWidget->selectedItems();
    auto _sdk = ui->sdk_listWidget->selectedItems();
    if (_db.size() != 1 && _sdk.size() != 1){
        auto* message_box = new QMessageBox();
        if (_db.empty()){
            message_box->critical(this, "Ошибка", "Вы не выбрали DB мод");
        }
        else if (_sdk.size() == 0){
            message_box->critical(this, "Ошибка", "Вы не выбрали SDK мод");
        }
        else if (_db.size() > 1){
            message_box->critical(this, "Ошибка", "Вы выбрали больше одного DB мода");
        }
        else {
            message_box->critical(this, "Ошибка", "Вы не выбрали DB мод");
        }
        message_box->exec();
        return;
    }
    int db_id =  ui->db_listWidget->row(_db[0]);
    int sdk_id = ui->sdk_listWidget->row(_sdk[0]);
    if (!mod_controller.exists_mods(installed_mods_db[db_id], installed_mods_sdk[sdk_id])){
        mod_controller.install_mods(installed_mods_db[db_id], installed_mods_sdk[sdk_id]);
    }
    mod_controller.update_mod(installed_mods_db[db_id], installed_mods_sdk[sdk_id]);
    QString path_game = QString::fromStdWString(app_settings.get_setting(L"path_game"));
    QString path_exe = path_game + "\\joshua.exe";
    QProcess::startDetached(path_exe,{"-server"}, path_game);
}

void MainWindow::on_pushButton_settings_clicked()
{
    if (!std::filesystem::exists(L"options.fl")){
        app_settings.generate_fl_file();
    }
    SettingsWindow* settings_window = new SettingsWindow(L"settings.fl", L"settings.ini");
    QObject::connect(settings_window, &SettingsWindow::resaved_file, &app_settings, &AppSettings::load);
    settings_window->show();
}


void MainWindow::on_MainWindow_destroyed()
{
    emit exit_app(0);
}


void MainWindow::on_toolButton_download_clicked()
{
    manager_window->show();
    manager_window->activateWindow();
}

void MainWindow::reload_mods() {
    reload_db();
    reload_sdk();
}

void MainWindow::reload_db() {
    installed_mods_db.clear();
    installed_mods_db.reserve(mod_controller.get_count_db());
    ui->db_listWidget->clear();
    int i = 0;
    for (auto & mod_db : mod_controller.get_mods_db()){
        QString name = QString::fromStdWString(mod_db.get_name());
        int j = 0;
        for (auto & version : mod_db.get_versions()){
            QString qversion = QString::fromStdWString(version);
            ui->db_listWidget->addItem(name + " " + qversion);
            installed_mods_db.emplace_back(i, j);
            j++;
        }
        i++;
    }
}

void MainWindow::reload_sdk() {
    installed_mods_sdk.clear();
    installed_mods_sdk.reserve(mod_controller.get_count_sdk());
    ui->sdk_listWidget->clear();
    int i = 0;
    for (auto & mod_sdk : mod_controller.get_mods_sdk()){
        QString name = QString::fromStdWString(mod_sdk.get_name());
        int j = 0;
        for (auto & version : mod_sdk.get_versions()){
            QString qversion = QString::fromStdWString(version);
            ui->sdk_listWidget->addItem(name + " " + qversion);
            installed_mods_sdk.emplace_back(i, j);
            j++;
        }
        i++;
    }
}

