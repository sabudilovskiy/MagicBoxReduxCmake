//
// Created by MRV on 8/6/2022.
//

#include "ModDownloader.h"
#include "Mod.h"
#include <magic_enum.hpp>
#include <QMessageBox>

void ModDownloader::download(DOWNLOAD_INFO info_download) {
    ModServer& downloading_mod = info_download.type == Mod::Type::DB ? _mods_db[info_download.ID_mod] : _mods_sdk[info_download.ID_mod];
    auto& version = downloading_mod.get_versions()[info_download.ID_version];
    std::wstring wpath = get_path(info_download);
    if (std::filesystem::exists(wpath)){
        QMessageBox msgBox;
        msgBox.setText("Обнаружен уже установленный мод");
        msgBox.setInformativeText("Вы хотите скачать заново " +
                                  QString::fromStdWString(downloading_mod.  get_name()) +
                                  " " + QString::fromStdWString(version) + "?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        if (ret == QMessageBox::No){
            Mod mod(downloading_mod.get_id(),
                    downloading_mod.get_name(),
                    {version},
                    downloading_mod.get_type());
            mod_controller.add_mod(mod);
            emit signal_downloaded_mod(info_download, dnetwork_error::NoError, dfile_error::NoError);
            return;
        }
        else {
            std::filesystem::remove_all(wpath);
        }
    }
    const std::wstring & wurl = downloading_mod.get_urls()[info_download.ID_version];
    QUrl url = QString::fromStdWString(wurl);
    _downloads[downloader.add_download(url, get_path_zip(info_download))] =  info_download;
}

void ModDownloader::downloaded_mod(dhash_t hash_item, dnetwork_error network_err, dfile_error file_err) {
    auto download = _downloads.find(hash_item);
    if (download == _downloads.end()){
        return;
    }
    auto info_download = download->second;
    if (network_err == dnetwork_error::NoError && file_err == dfile_error::NoError){
        std::string path = string_from_wstring(get_path(info_download));
        std::string path_zip = string_from_wstring(get_path_zip(info_download));
        unpack_mod_zip(path_zip, path);
        std::filesystem::remove(path_zip);
        auto & mods = info_download.type == Mod::Type::DB ? _mods_db : _mods_sdk;
        auto & downloaded_mod = mods[info_download.ID_mod];
        Mod mod(downloaded_mod.get_id(),
                downloaded_mod.get_name(),
                {downloaded_mod.get_versions()[info_download.ID_version]},
                downloaded_mod.get_type());
        mod_controller.add_mod(mod);
        downloaded_mod.get_installed()[info_download.ID_version] = true;
    }
    emit signal_downloaded_mod(info_download, network_err, file_err);
}

std::wstring ModDownloader::get_path(DOWNLOAD_INFO info_download) {
    ModServer* mod;
    if (info_download.type == Mod::Type::DB){
        mod = &_mods_db[info_download.ID_mod];
    }
    else mod = &_mods_sdk[info_download.ID_mod];
    std::wstring path = L"mods";
    if (mod->get_type() == Mod::Type::DB){
        path+=L"\\DB\\";
    }
    else path+=L"\\SDK\\";
    path+=mod->get_id();
    path+=L'\\';
    path+= mod->get_versions()[info_download.ID_version];
    return path;
}

std::wstring ModDownloader::get_path_zip(DOWNLOAD_INFO info_download) {
    ModServer* mod;
    if (info_download.type == Mod::Type::DB){
        mod = &_mods_db[info_download.ID_mod];
    }
    else mod = &_mods_sdk[info_download.ID_mod];
    std::wstring path = L"downloads\\";
    path+=mod->get_id() + L'_' + mod->get_versions()[info_download.ID_version] + L".zip";
    return path;
}

ModDownloader::ModDownloader(const std::wstring &path_config, Downloader &downloader, ModController &mod_controller)
        : downloader(downloader), mod_controller(mod_controller), _path_config(path_config){
    load_from_xml();
    connect();
}

const std::vector<ModServer> &ModDownloader::get_mods_db() {
    return _mods_db;
}

const std::vector<ModServer> &ModDownloader::get_mods_sdk() {
    return _mods_sdk;
}

void ModDownloader::downloaded_config(dhash_t hash_item, dnetwork_error network_err, dfile_error file_err) {
    QObject::disconnect(
            &downloader,
            &Downloader::downloaded_file,
            this,
            &ModDownloader::downloaded_config);
    if (network_err==dnetwork_error::NoError && file_err==dfile_error::NoError){
        load_from_xml();
        emit refreshed_list();
    }
    connect();
}

ModDownloader::ModDownloader(const std::wstring &url, const std::wstring &path_config, Downloader &downloader,
                             ModController &mod_controller) : _path_config(path_config), downloader(downloader), mod_controller(mod_controller){
    auto url_query = QString::fromStdWString(url).split('?');
    QUrl qurl(url_query[0]);
    if (url_query.size() > 1) qurl.setQuery(url_query[1]);
    QObject::connect(
            &downloader,
            &Downloader::downloaded_file,
            this,
            &ModDownloader::downloaded_config);
    downloader.add_download(qurl, path_config);
}

void ModDownloader::load_from_xml() {
    std::wifstream file (_path_config);
    file.imbue(std::locale("ru_RU.UTF-8"));
    if (file.is_open()){
        XML xml(file);
        auto mods_node = xml.get_child(L"mods");
        if (mods_node){
            for (const auto& cur_mod : *mods_node){
                ModServer mod(cur_mod.first, *cur_mod.second);
                if (mod.get_type() == Mod::DB){
                    _mods_db.push_back(std::move(mod));
                }
                else {
                    _mods_sdk.push_back(std::move(mod));
                }
            }
        }
    }
    std::sort(_mods_db.begin(), _mods_db.end());
    std::sort(_mods_sdk.begin(), _mods_sdk.end());
    update_downloaded();
    emit refreshed_list();
}

void ModDownloader::clear() {
    _mods_db.clear();
    _mods_sdk.clear();
}

void ModDownloader::save_to_xml() {
    std::wofstream file (_path_config);
    file.imbue(std::locale("ru_RU.UTF-8"));
    std::sort(_mods_db.begin(), _mods_db.end());
    std::sort(_mods_sdk.begin(), _mods_sdk.end());
    if (file.is_open()){
        XML xml;
        auto mods_node = xml.add_child(L"mods");
        if (mods_node){
            for (auto& mod : _mods_db){
                auto mod_node = mods_node->add_child(mod.get_id());
                mod_node->add_child(L"name", mod.get_name());
                mod_node->add_child(L"type", wstring_from_string(std::string(magic_enum::enum_name(mod.get_type()))));
                auto versions_node = mod_node->add_child(L"versions");
                int i = 0;
                for (auto& version : mod.get_versions()){
                    i++;
                    versions_node->add_child(L"v" + std::to_wstring(i), version);
                }
                i = 0;
                auto urls_node = mod_node->add_child(L"urls");
                for (auto& url : mod.get_urls()){
                    i++;
                    urls_node->add_child(L"u" + std::to_wstring(i), url);
                }
            }
        }
        xml.save_to_file(file);
    }
}

ModDownloader::~ModDownloader() {
    save_to_xml();
}

void ModDownloader::update_downloaded() {
    auto it_installed_mods = mod_controller.get_mods_db().begin(), end_installed_mods = mod_controller.get_mods_db().end();
    auto it_server_mods = _mods_db.begin(), end_server_mods = _mods_db.end();
    while (it_server_mods!=end_server_mods && it_installed_mods!=end_installed_mods){
        if (it_server_mods->get_id() < it_installed_mods->get_id() ){
            while (it_server_mods!=end_server_mods && it_server_mods->get_id() < it_installed_mods->get_id() ){
                it_server_mods++;
            }
        }
        else if (it_server_mods->get_id() == it_installed_mods->get_id()){
            size_t n = it_server_mods->get_versions().size();
            for (size_t i = 0; i < n; i++){
                if (include(it_installed_mods->get_versions(), it_server_mods->get_versions()[i])){
                    it_server_mods->get_installed()[i] = true;
                }
            }
            it_server_mods++;
        }
        else{
            while(it_installed_mods!=end_installed_mods &&it_server_mods->get_id() > it_installed_mods->get_id()){
                it_installed_mods++;
            }
        }
    }
    it_installed_mods = mod_controller.get_mods_sdk().begin(), end_installed_mods = mod_controller.get_mods_sdk().end();
    it_server_mods = _mods_sdk.begin(), end_server_mods = _mods_sdk.end();
    while (it_server_mods!=end_server_mods && it_installed_mods!=end_installed_mods){
        if (it_server_mods->get_id() < it_installed_mods->get_id() ){
            while (it_server_mods!=end_server_mods && it_server_mods->get_id() < it_installed_mods->get_id() ){
                it_server_mods++;
            }
        }
        else if (it_server_mods->get_id() == it_installed_mods->get_id()){
            size_t n = it_server_mods->get_versions().size();
            for (size_t i = 0; i < n; i++){
                if (include(it_installed_mods->get_versions(), it_server_mods->get_versions()[i])){
                    it_server_mods->get_installed()[i] = true;
                }
            }
            it_server_mods++;
        }
        else{
            while(it_installed_mods!=end_installed_mods &&it_server_mods->get_id() > it_installed_mods->get_id()){
                it_installed_mods++;
            }
        }
    }
}

bool ModDownloader::installed(DOWNLOAD_INFO info_download) const {
    auto& mods = info_download.type == Mod::DB ? _mods_db : _mods_sdk;
    return mods[info_download.ID_mod].get_installed()[info_download.ID_version];
}

void ModDownloader::start_downloading(dhash_t hash_item) {
    auto download = _downloads.find(hash_item);
    if (download == _downloads.end()){
        return;
    }
    emit signal_downloading_mod(download->second);
}

void ModDownloader::connect() {
    QObject::connect(&downloader,
                     &Downloader::downloaded_file,
                     this,
                     &ModDownloader::downloaded_mod);
    QObject::connect(&downloader,
                     &Downloader::start_downloading,
                     this,
                     &ModDownloader::start_downloading);
    QObject::connect(&downloader,
                     &Downloader::downloaded_progress,
                     this,
                     &ModDownloader::downloading_progress);
}

void ModDownloader::add_mod(ModServer mod) {
    auto& mods = mod.get_type() == Mod::DB ? _mods_db : _mods_sdk;
    mods.push_back(std::move(mod));
}

DOWNLOAD_INFO::DOWNLOAD_INFO(int id_mod, int id_version, Mod::Type type) : ID_mod(id_mod), ID_version(id_version),
                                                                           type(type) {}

DOWNLOAD_INFO::DOWNLOAD_INFO(const DOWNLOAD_INFO &another) : ID_mod(another.ID_mod), ID_version(another.ID_version), type(another.type){

}
