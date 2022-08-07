//
// Created by MRV on 8/6/2022.
//

#ifndef MAGICBOXREDUXCMAKE_MODDOWNLOADER_H
#define MAGICBOXREDUXCMAKE_MODDOWNLOADER_H

#include "ModServer.h"
#include "Downloader.h"
#include <unordered_map>
#include <filesystem>
#include <libzippp/libzippp.h>
#include "Support.h"

class ModDownloader : public QObject {
protected:
    Q_OBJECT
    Downloader& downloader;
    std::vector<ModServer> mods_db;
    std::vector<ModServer> mods_sdk;
    struct INFO_DOWNLOAD{
        int ID_mod;
        int ID_version;
        Mod::Type type;
    };
    std::unordered_map<dhash_t, INFO_DOWNLOAD> downloads;
    const std::wstring & path_config;
public:
    //класс загрузит из файла
    ModDownloader(const std::wstring& path_config, Downloader& downloader);
    //класс сам загрузит нужные данные
    ModDownloader(const std::wstring& url, const std::wstring& path_config, Downloader& downloader);
    ~ModDownloader();
    const std::vector<ModServer>& get_mods_db();
    const std::vector<ModServer>& get_mods_sdk();
protected:
    std::wstring get_path_zip(INFO_DOWNLOAD info_download);
    std::wstring get_path(INFO_DOWNLOAD info_download);
    void load_from_xml();
    void save_to_xml();
    void clear();
protected slots:
    void downloaded_mod(dhash_t hash_item, dnetwork_error network_err, dfile_error file_err);
    void downloaded_config(dhash_t hash_item, dnetwork_error network_err, dfile_error file_err);
public:
    void download(INFO_DOWNLOAD info_download);
    signals:
    void downloaded_mod(INFO_DOWNLOAD info_download, dnetwork_error network_err, dfile_error file_err);
};


#endif //MAGICBOXREDUXCMAKE_MODDOWNLOADER_H
