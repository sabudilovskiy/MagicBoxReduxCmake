//
// Created by MRV on 8/6/2022.
//

#ifndef MAGICBOXREDUXCMAKE_MODDOWNLOADER_H
#define MAGICBOXREDUXCMAKE_MODDOWNLOADER_H

#include"ModServer.h"
#include "Downloader.h"
#include <unordered_map>
#include <filesystem>
#include <libzippp/libzippp.h>
#include "Support.h"
#include "ModController.h"
struct DOWNLOAD_INFO{
    int ID_mod;
    int ID_version;
    Mod::Type type;
    DOWNLOAD_INFO(){}
    DOWNLOAD_INFO(const DOWNLOAD_INFO& another);
    DOWNLOAD_INFO(int id_mod, int id_version, Mod::Type type);
};
class ModDownloader : public QObject {
protected:
    Q_OBJECT
    Downloader& downloader;
    ModController& mod_controller;
    std::vector<ModServer> _mods_db;
    std::vector<ModServer> _mods_sdk;
    std::unordered_map<dhash_t, DOWNLOAD_INFO> _downloads;
    const std::wstring & _path_config;
public:
    //класс загрузит из файла
    ModDownloader(const std::wstring &path_config, Downloader &downloader, ModController &mod_controller);
    //класс сам загрузит нужные данные
    ModDownloader(const std::wstring &url, const std::wstring &path_config, Downloader &downloader,
                  ModController &mod_controller);
    ~ModDownloader();
    void add_mod(ModServer mod);
    const std::vector<ModServer>& get_mods_db();
    const std::vector<ModServer>& get_mods_sdk();
protected:
    std::wstring get_path_zip(DOWNLOAD_INFO info_download);
    std::wstring get_path(DOWNLOAD_INFO info_download);
    void clear();
    void connect();
protected slots:
    void downloaded_mod(dhash_t hash_item, dnetwork_error network_err, dfile_error file_err);
    void start_downloading(dhash_t hash_item);
    void downloaded_config(dhash_t hash_item, dnetwork_error network_err, dfile_error file_err);
public:
    void download(DOWNLOAD_INFO info_download);
    void save_to_xml();
    void load_from_xml();
    void update_downloaded();
    bool installed(DOWNLOAD_INFO info_download) const;
    signals:
    void refreshed_list();
    void downloading_progress(qint64 downloaded, qint64 all);
    void signal_downloading_mod(DOWNLOAD_INFO info);
    void signal_downloaded_mod(DOWNLOAD_INFO mod, dnetwork_error network_err, dfile_error file_err);
};


#endif //MAGICBOXREDUXCMAKE_MODDOWNLOADER_H
