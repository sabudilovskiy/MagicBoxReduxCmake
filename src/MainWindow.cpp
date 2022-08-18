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
        downloader(downloader),
        app_settings(app_settings),
        ui(new Ui::MainWindow),
        mod_controller(app_settings.get_setting(L"path_game"),
                       app_settings.get_setting(L"path_config_ModController")),
        mod_downloader(app_settings.get_setting(L"url_config_ModDownloader"),
                       app_settings.get_setting(L"path_config_ModDownloader"),
                       downloader, mod_controller)
{
    if (parent) this->setParent(parent);
    this->manager_window = std::make_unique<DownloaderWindow>(app_settings, mod_downloader);
    ui->setupUi(this);
    reload_mods();
    QObject::connect(&mod_controller, &ModController::change_db, this, &MainWindow::reload_db);
    QObject::connect(&mod_controller, &ModController::change_sdk, this, &MainWindow::reload_sdk);
    QObject::connect(ui->toolButton_minimize, &QToolButton::clicked, this, &MainWindow::showMinimized);
    QObject::connect(ui->toolButton_exit, &QToolButton::clicked, this, &MainWindow::close);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_rightClicked_db(const QPoint &pos) {

}

void MainWindow::on_rightClicked_sdk(const QPoint &pos) {

}

void MainWindow::on_toolButton_set_reinstall_clicked() {
    auto [db_id, sdk_id] = get_set();
    if (db_id == -1){
        return;
    }
    std::wstring full_path = app_settings.get_setting(L"path_game");
    full_path+=L"/MODS/";
    full_path+=mod_controller.get_name_folder(installed_mods_db[db_id], installed_mods_sdk[sdk_id]);
    if (std::filesystem::exists(full_path)){
        std::filesystem::remove_all(full_path);
    }
    mod_controller.install_mods(installed_mods_db[db_id], installed_mods_sdk[sdk_id]);
    QString text = "Сборка состоящая из ";
    text += ui->db_listWidget->item(db_id)->text();
    text += " и ";
    text += ui->sdk_listWidget->item(sdk_id)->text();
    text += " успешна переустановлена";
    QMessageBox::information(this, "Переустановлена сборка", text);
}

void MainWindow::on_pushButton_start_game_clicked() {
    auto [db_id, sdk_id] = get_set();
    if (db_id == -1){
        return;
    }
    if (!mod_controller.exists_mods(installed_mods_db[db_id], installed_mods_sdk[sdk_id])){
        mod_controller.install_mods(installed_mods_db[db_id], installed_mods_sdk[sdk_id]);
    }
    mod_controller.update_mod(installed_mods_db[db_id], installed_mods_sdk[sdk_id]);
    QString path_game = QString::fromStdWString(app_settings.get_setting(L"path_game"));
    QString path_exe = path_game + "\\joshua.exe";
    QProcess::startDetached(path_exe,{}, path_game);
}


void MainWindow::on_pushButton_start_server_clicked()
{
    auto [db_id, sdk_id] = get_set();
    if (db_id == -1){
        return;
    }
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
    _settings_window = new SettingsWindow(L"", L"settings.fl", "Настройки MagicBoxRedux");
    QObject::connect(_settings_window, &SettingsWindow::resaved_file, &app_settings, &AppSettings::load);
    _settings_window->show();
}


void MainWindow::on_MainWindow_destroyed()
{
    emit exit_app();
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


void MainWindow::on_toolButton_set_config_clicked()
{
    auto [db_id, sdk_id] = get_set();
    if (db_id == -1){
        return;
    }
    if (!mod_controller.exists_mods(installed_mods_db[db_id], installed_mods_sdk[sdk_id])){
        mod_controller.install_mods(installed_mods_db[db_id], installed_mods_sdk[sdk_id]);
    }
    close_settings_window();
    std::wstring full_path = app_settings.get_setting(L"path_game");
    full_path+=L"/MODS/";
    full_path+=mod_controller.get_name_folder(installed_mods_db[db_id], installed_mods_sdk[sdk_id]);
    QString text = "Настройки ";
    text += ui->db_listWidget->item(db_id)->text();
    text += " + ";
    text += ui->sdk_listWidget->item(sdk_id)->text();
    _settings_window = new SettingsWindow(full_path, L"mod.fl",  text);
    _settings_window->show();
}

std::pair<int, int> MainWindow::get_set() {
    auto _db = ui->db_listWidget->selectedItems();
    auto _sdk = ui->sdk_listWidget->selectedItems();
    if (_db.size() != 1 || _sdk.size() != 1){
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
        return {-1, -1};
    }
    return {ui->db_listWidget->row(_db[0]), ui->sdk_listWidget->row(_sdk[0])};
}

void MainWindow::close_settings_window() {
    if (!_settings_window){
        return;
    }
    delete _settings_window;
    _settings_window = nullptr;
}

void MainWindow::on_toolButton_set_delete_clicked() {
    auto [db_id, sdk_id] = get_set();
    if (db_id == -1){
        return;
    }
    std::wstring full_path = app_settings.get_setting(L"path_game");
    full_path+=L"/MODS/";
    full_path+=mod_controller.get_name_folder(installed_mods_db[db_id], installed_mods_sdk[sdk_id]);
    if (std::filesystem::exists(full_path)){
        std::error_code errc;;
        std::filesystem::remove_all(full_path,errc);
        if (errc){
            QMessageBox::critical(this, "Ошибка", QString::fromStdString(errc.message()));
        }
        else{
            QString text = "Сборка состоящая из ";
            text += ui->db_listWidget->item(db_id)->text();
            text += " и ";
            text += ui->sdk_listWidget->item(sdk_id)->text();
            text += " успешна удалена";
            QMessageBox::information(this, "Удалена сборка", text);
        }
    }
    else{
        QString text = "Сборка состоящая из ";
        text += ui->db_listWidget->item(db_id)->text();
        text += " и ";
        text += ui->sdk_listWidget->item(sdk_id)->text();
        text += " не была удалена, так как она не была установлена.";
        QMessageBox::information(this, "Ошибка", std::move(text));
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (mod_downloader.number_downloadings() > 0){
        QMessageBox msgBox;
        msgBox.setText("Выполняется скачивание");
        msgBox.setInformativeText("Закрыть Magic box?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        if (ret == QMessageBox::Yes){
            event->accept();
            emit exit_app();
        }
    }
    else
    {
        event->accept();
        emit exit_app();
    }
    event->ignore();
}

