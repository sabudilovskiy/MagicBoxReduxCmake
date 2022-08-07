//
// Created by MRV on 8/6/2022.
//

#include "ModDownloader.h"

void ModDownloader::download(ModDownloader::INFO_DOWNLOAD info_download) {
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
    std::string path = string_from_wstring(get_path(info_download));
    std::string path_zip = string_from_wstring(get_path_zip(info_download));
    unpack_mod_zip(path_zip, path);
}

std::wstring ModDownloader::get_path(ModDownloader::INFO_DOWNLOAD info_download) {
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

std::wstring ModDownloader::get_path_zip(ModDownloader::INFO_DOWNLOAD info_download) {
    ModServer* mod;
    if (info_download.type == Mod::Type::DB){
        mod = &mods_db[info_download.ID_mod];
    }
    else mod = &mods_sdk[info_download.ID_mod];
    std::wstring path = L"downloads\\";
    path+=mod->get_id() + L'_' + mod->get_versions()[info_download.ID_version] + L".zip";
    return path;
}

ModDownloader::ModDownloader(const std::wstring& path_config, Downloader &downloader) : path_config(path_config), downloader(downloader){
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
    QObject::disconnect(&downloader, SIGNAL(
            downloaded_file(dhash_t hash_item, dnetwork_error network_err, dfile_error file_err)),
                        this,
                        SLOT(downloaded_config(dhash_t hash_item, dnetwork_error network_err, dfile_error file_err)));
    if (network_err==dnetwork_error::NoError && file_err==dfile_error::NoError){
        load_from_xml();
    }
    QObject::connect(&downloader,
                     SIGNAL(downloaded_file(dhash_t, dnetwork_error, dfile_error)),
                     this,
                     SLOT(downloaded_mod(dhash_t, dnetwork_error, dfile_error)));

}

ModDownloader::ModDownloader(const std::wstring& url, const std::wstring &path_config, Downloader &downloader) : path_config(path_config), downloader(downloader){
    QUrl qurl(QString::fromStdWString(url));
    QObject::connect(
            &downloader,
            SIGNAL(downloaded_file(dhash_t hash_item, dnetwork_error network_err, dfile_error file_err)),
            this,
            SLOT(downloaded_config(dhash_t hash_item, dnetwork_error network_err, dfile_error file_err)));
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
}

void ModDownloader::clear() {
    mods_db.clear();
    mods_sdk.clear();
}

void ModDownloader::save_to_xml() {
    std::wofstream file (path_config);
    file.imbue(std::locale("ru_RU.UTF-8"));
    if (file.is_open()){
        XML xml;
        auto mods_node = xml.add_child(L"mods");
        if (mods_node){
            for (auto& mod : mods_db){
                auto cur_node = mods_node->add_child(mod.get_id());
                cur_node->add_child(L"name", mod.get_name());

            }
        }
    }
}

ModDownloader::~ModDownloader() {
    save_to_xml();
}
