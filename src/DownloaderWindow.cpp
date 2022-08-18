#include "DownloaderWindow.h"
#include "ui_DownloaderWindow.h"
#define MAGIC_ENUM_RANGE_MIN 0
#define MAGIC_ENUM_RANGE_MAX 500
#include <magic_enum.hpp>
#include <QMessageBox>
DownloaderWindow::~DownloaderWindow()
{
    delete ui;
}

DownloaderWindow::DownloaderWindow(const AppSettings& app_settings, ModDownloader &mod_downloader, QWidget *parent) :
        _app_settings(app_settings),
        QMainWindow(parent),
        _mod_downloader(mod_downloader),
        ui(new Ui::DownloaderWindow)
{
    ui->setupUi(this);
    QObject::connect(&mod_downloader, &ModDownloader::refreshed_list, this, &DownloaderWindow::update_mod);
    update_mod();
    update_settings();
    QObject::connect(&mod_downloader, &ModDownloader::signal_downloaded_mod, this, &DownloaderWindow::downloaded_mod);
    QObject::connect(&mod_downloader, &ModDownloader::downloading_progress, this, &DownloaderWindow::update_bytes_progress_bar);
    QObject::connect(&mod_downloader, &ModDownloader::signal_downloading_mod, this, &DownloaderWindow::downloading_mod);
    QObject::connect(&mod_downloader, &ModDownloader::refreshed_list, this, &DownloaderWindow::update_mod);
    QObject::connect(&app_settings, &AppSettings::change_settings, this, &DownloaderWindow::update_settings);
    QObject::connect(ui->toolButton_minimize, &QToolButton::clicked, this, &DownloaderWindow::showMinimized);
    QObject::connect(ui->toolButton_exit, &QToolButton::clicked, this, &DownloaderWindow::close);
}


void DownloaderWindow::update_mod() {
    auto count_db = _mod_downloader.get_mods_db().size();
    auto count_sdk = _mod_downloader.get_mods_sdk().size();
    _mods_db.clear();
    _mods_sdk.clear();
    _mods_db.reserve(count_db);
    _mods_sdk.reserve(count_sdk);
    ui->listWidget_db->clear();
    ui->listWidget_sdk->clear();
    int id_mod = 0;
    for (auto& mod : _mod_downloader.get_mods_db()){
        int id_version = 0;
        auto& name_mod = mod.get_name();
        for (auto & version : mod.get_versions()){
            std::wstring full_name = name_mod;
            full_name+=L' ';
            full_name+=version;
            if (mod.get_installed()[id_version]){
                auto* item = new QListWidgetItem(QString::fromStdWString(full_name), ui->listWidget_db);
                QFont font("Segoe UI", 9, -1, true);
                font.setBold(true);
                item->setFont(font);
                ui->listWidget_db->addItem(item);
            }
            else ui->listWidget_db->addItem(QString::fromStdWString(full_name));
            _mods_db.emplace_back(id_mod, id_version);
            ++id_version;
        }
        ++id_mod;
    }
    id_mod = 0;
    for (auto& mod : _mod_downloader.get_mods_sdk()){
        size_t id_version = 0;
        auto& name_mod = mod.get_name();
        for (auto & version : mod.get_versions()){
            std::wstring full_name = name_mod;
            full_name+=L' ';
            full_name+=version;
            if (mod.get_installed()[id_version]){
                auto* item = new QListWidgetItem(QString::fromStdWString(full_name), ui->listWidget_sdk);
                QFont font("Segoe UI", 9, -1, true);
                font.setBold(true);
                item->setFont(font);
                ui->listWidget_sdk->addItem(item);
            }
            else ui->listWidget_sdk->addItem(QString::fromStdWString(full_name));
            _mods_sdk.emplace_back(id_mod, id_version);
            ++id_version;
        }
        ++id_mod;
    }
}

void DownloaderWindow::on_pushButton_download_clicked()
{
    auto items_db = ui->listWidget_db->selectedItems();
    for (auto & item_db: items_db){
        int ID = ui->listWidget_db->row(item_db);
        auto info = _mods_db[ID];
        if (_mod_downloader.installed({info.id_mod, info.id_version, Mod::DB})){
            switch (_installed_policy) {
                case SKIP:
                    break;
                case REINSTALL:
                    add_downloading();
                    _mod_downloader.download({info.id_mod, info.id_version, Mod::DB});
                    break;
                case ASK:
                    {
                        QMessageBox msgBox;
                        msgBox.setText("Обнаружен уже установленный мод");
                        msgBox.setInformativeText("Вы хотите переустановить " + item_db->text() + " ?");
                        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                        msgBox.setDefaultButton(QMessageBox::No);
                        int ret = msgBox.exec();
                        if (ret == QMessageBox::Yes){
                            add_downloading();
                            _mod_downloader.download({info.id_mod, info.id_version, Mod::DB});
                        }
                    }
                    break;
            }
        }
        else {
            add_downloading();
            _mod_downloader.download({info.id_mod, info.id_version, Mod::DB});
        }
    }
    auto items_sdk = ui->listWidget_sdk->selectedItems();
    for (auto & item_sdk: items_sdk){
        int ID = ui->listWidget_sdk->row(item_sdk);
        auto info = _mods_sdk[ID];
        if (_mod_downloader.installed({info.id_mod, info.id_version, Mod::SDK})){
            switch (_installed_policy) {
                case SKIP:
                    break;
                case REINSTALL:
                    add_downloading();
                    _mod_downloader.download({info.id_mod, info.id_version, Mod::SDK});
                    break;
                case ASK:
                {
                    QMessageBox msgBox;
                    msgBox.setText("Обнаружен уже установленный мод");
                    msgBox.setInformativeText("Вы хотите переустановить " + item_sdk->text() + " ?");
                    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                    msgBox.setDefaultButton(QMessageBox::No);
                    int ret = msgBox.exec();
                    if (ret == QMessageBox::Yes){
                        add_downloading();
                        _mod_downloader.download({info.id_mod, info.id_version, Mod::SDK});
                    }
                }
                break;
            }
        }
        else {
            add_downloading();
            _mod_downloader.download({info.id_mod, info.id_version, Mod::SDK});
        }
    }
}

void DownloaderWindow::downloaded_mod(DOWNLOAD_INFO mod, dnetwork_error network_err, dfile_error file_err) {
    auto& list = mod.type == Mod::DB ? ui->listWidget_db: ui->listWidget_sdk;
    auto item = list->item(get_index(mod));
    QString text = item->text();
    if (network_err == dnetwork_error::NoError && file_err == dfile_error::NoError){
        text+=" скачан успешно.";
        QFont font("Segoe UI", 9, -1, true);
        font.setBold(true);
        item->setFont(font);
    }
    else{
        text+=" не удалось скачать. Ошибка: ";
        if (network_err != dnetwork_error::NoError) {
            std::string_view name_error = magic_enum::enum_name<dnetwork_error>(network_err);
            text += QString::fromStdString(std::string(name_error.data()));
        }
        else {
            text += QString::fromStdString(std::string(magic_enum::enum_name(file_err).data()));
        }
    }
    ui->label_downloaded_mod->setText(text);
    add_downloaded();
}

void DownloaderWindow::update_settings() {
    _installed_policy = magic_enum::enum_cast<InstalledPolicy>(string_from_wstring(_app_settings.get_setting(L"policies_downloadInstalledMods"))).value();
}

void DownloaderWindow::add_downloading() {
    if (_downloaded == _downloading){
        _downloading = 1;
        _downloaded = 0;
    }
    else {
        ++_downloading;
    }
    update_mod_progress_bar();
}

void DownloaderWindow::add_downloaded() {
    ++_downloaded;
    update_mod_progress_bar();
}

 int DownloaderWindow::get_index(DOWNLOAD_INFO mod) const{
    auto& mods = mod.type == Mod::Type::DB ? _mods_db: _mods_sdk;
    ModVersion mod_version{mod.ID_mod, mod.ID_version};
    return std::distance(mods.begin(), std::find(mods.begin(),  mods.end(), mod_version));
}

void DownloaderWindow::update_mod_progress_bar() {
    ui->progressBar_mods->setMaximum(_downloading);
    ui->progressBar_mods->setValue(_downloaded);
    ui->label_mods->setText(QString::number(_downloaded) + "/" + QString::number(_downloading));
}

void DownloaderWindow::downloading_mod(DOWNLOAD_INFO mod) {
    auto& list = mod.type == Mod::DB ? ui->listWidget_db: ui->listWidget_sdk;
    auto item = list->item(get_index(mod));
    QString text = "Скачивается: ";
    text+=item->text();
    ui->label_downloading_mod->setText(text);
}

void DownloaderWindow::update_bytes_progress_bar(qint64 downloaded_bytes, qint64 total_bytes) {
    if (total_bytes == -1){
        return;
    }
    if (downloaded_bytes == -1){
        downloaded_bytes = 0;
    }
    QString after;
    double total;
    double downloaded;
    if (total_bytes > 1024 * 1024 * 10){
        total = total_bytes/1024/1024;
        total = std::round(total*10)/10;
        downloaded = downloaded_bytes/1024/1024;
        downloaded = round(downloaded*10)/10;
        after = "MB";
    }
    else if (total_bytes > 1024 * 10){
        total = total_bytes/1024;
        total = std::round(total*10)/10;
        downloaded = downloaded_bytes/1024;
        downloaded = round(downloaded*10)/10;
        after = "KB";
    }
    else{
        downloaded = downloaded_bytes;
        total = total_bytes;
        after = "B";
    }
    QString text = QString::number(downloaded);
    text += '/';
    text += QString::number(total);
    text += ' ';
    text += after;
    ui->label_bytes->setText(text);
    ui->progressBar_bytes->setMaximum(total);
    ui->progressBar_bytes->setValue(downloaded);
}

void DownloaderWindow::on_toolButton_refresh_clicked() {
    if (_mod_downloader.number_downloadings() > 0){
        QMessageBox* message_box;
        message_box->warning(this, "Невозможно обновить доступные моды для скачивания", "Невозможно обновить доступные моды для скачивания, так как есть незавершённые загрузки");
    }
    else _mod_downloader.refresh_config();
}



