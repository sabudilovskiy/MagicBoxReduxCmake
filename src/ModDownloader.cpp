//
// Created by MRV on 8/6/2022.
//

#include "ModDownloader.h"
#include "Mod.h"
#include <magic_enum.hpp>
void ModDownloader::download(DOWNLOAD_INFO info_download) {
    ModServer* mod;
    if (info_download.type == Mod::Type::DB){
        mod = &mods_db[info_download.ID_mod];
    }
    else mod = &mods_sdk[info_download.ID_mod];
    const std::wstring & wurl = mod->get_urls()[info_download.ID_version];
    QUrl url = QString::fromStdWString(wurl);
    downloads.emplace(downloader.add_download(url, get_path_zip(info_download)), info_download);
}

void ModDownloader::downloaded_mod(dhash_t hash_item, dnetwork_error network_err, dfile_error file_err) {
    auto download = downloads.find(hash_item);
    if (download==downloads.end()){
        return;
    }
    auto info_download = download->second;
    if (network_err == dnetwork_error::NoError && file_err == dfile_error::NoError){
        std::string path = string_from_wstring(get_path(info_download));
        std::string path_zip = string_from_wstring(get_path_zip(info_download));
        unpack_mod_zip(path_zip, path);
        auto & mods = info_download.type == Mod::Type::DB ? mods_db : mods_sdk;
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
        mod = &mods_db[info_download.ID_mod];
    }
    else mod = &mods_sdk[info_download.ID_mod];
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
        mod = &mods_db[info_download.ID_mod];
    }
    else mod = &mods_sdk[info_download.ID_mod];
    std::wstring path = L"downloads\\";
    path+=mod->get_id() + L'_' + mod->get_versions()[info_download.ID_version] + L".zip";
    return path;
}

ModDownloader::ModDownloader(const std::wstring &path_config, Downloader &downloader, ModController &mod_controller)
        : downloader(downloader), mod_controller(mod_controller), path_config(path_config){
    load_from_xml();
    QObject::connect(&downloader,
                     SIGNAL(downloaded_file(dhash_t, dnetwork_error, dfile_error)),
                     this,
                     SLOT(downloaded_mod(dhash_t, dnetwork_error, dfile_error)));
}

const std::vector<ModServer> &ModDownloader::get_mods_db() {
    return mods_db;
}

const std::vector<ModServer> &ModDownloader::get_mods_sdk() {
    return mods_sdk;
}

void ModDownloader::downloaded_config(dhash_t hash_item, dnetwork_error network_err, dfile_error file_err) {
    QObject::disconnect(
            &downloader,
            &Downloader::downloaded_file,
            this,
            &ModDownloader::downloaded_config);
    if (network_err==dnetwork_error::NoError && file_err==dfile_error::NoError){
        load_from_xml();
    }
    QObject::connect(
            &downloader,
            &Downloader::downloaded_file,
            this,
            &ModDownloader::downloaded_mod);

}

ModDownloader::ModDownloader(const std::wstring &url, const std::wstring &path_config, Downloader &downloader,
                             ModController &mod_controller) : path_config(path_config), downloader(downloader), mod_controller(mod_controller){
    QUrl qurl(QString::fromStdWString(url));
    QObject::connect(
            &downloader,
            &Downloader::downloaded_file,
            this,
            &ModDownloader::downloaded_config);
    downloader.add_download(qurl, path_config);
}

void ModDownloader::load_from_xml() {
    std::wifstream file (path_config);
    file.imbue(std::locale("ru_RU.UTF-8"));
    if (file.is_open()){
        XML xml(file);
        auto mods_node = xml.get_child(L"mods");
        if (mods_node){
            for (const auto& cur_mod : *mods_node){
                ModServer mod(cur_mod.first, *cur_mod.second);
                if (mod.get_type() == Mod::DB){
                    mods_db.push_back(std::move(mod));
                }
                else {
                    mods_sdk.push_back(std::move(mod));
                }
            }
        }
    }
    std::sort(mods_db.begin(),  mods_db.end());
    std::sort(mods_sdk.begin(),  mods_sdk.end());
    update_downloaded();
    emit refreshed_list();
}

void ModDownloader::clear() {
    mods_db.clear();
    mods_sdk.clear();
}

void ModDownloader::save_to_xml() {
    std::wofstream file (path_config);
    file.imbue(std::locale("ru_RU.UTF-8"));
    std::sort(mods_db.begin(),  mods_db.end());
    std::sort(mods_sdk.begin(),  mods_sdk.end());
    if (file.is_open()){
        XML xml;
        auto mods_node = xml.add_child(L"mods");
        if (mods_node){
            for (auto& mod : mods_db){
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
    auto it_server_mods = mods_db.begin(), end_server_mods = mods_db.end();
    while (it_server_mods!=end_server_mods && it_installed_mods!=end_installed_mods){
        while (it_server_mods!=end_server_mods && it_server_mods->get_id() < it_installed_mods->get_id() ){
            it_server_mods++;
        }
        if (it_server_mods->get_id() == it_installed_mods->get_id()){
            size_t n = it_server_mods->get_versions().size();
            for (size_t i = 0; i < n; i++){
                if (include(it_installed_mods->get_versions(), it_server_mods->get_versions()[i])){
                    it_server_mods->get_installed()[i] = true;
                }
            }
        }
        it_server_mods++;
        while(it_installed_mods!=end_installed_mods &&it_server_mods->get_id() > it_installed_mods->get_id()){
            it_installed_mods++;
        }
    }
}

bool ModDownloader::installed(DOWNLOAD_INFO info_download) const {
    auto& mods = info_download.type == Mod::DB ? mods_db : mods_sdk;
    return mods[info_download.ID_mod].get_installed()[info_download.ID_version];
}
