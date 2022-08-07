//
// Created by MRV on 8/5/2022.
//

#ifndef MAGICBOXREDUXCMAKE_MODCONTROLLER_H
#define MAGICBOXREDUXCMAKE_MODCONTROLLER_H

#include "Mod.h"
#include <vector>
#include <QString>
#include <QObject>
class ModController : QObject {
    std::vector<Mod> mods_db;
    std::vector<Mod> mods_sdk;
    const std::wstring& path_game;
    const std::wstring& path_xml;
    Q_OBJECT
protected:
    void load_from_xml(const XML& xml);
public:
    ModController(const std::wstring& path_game, const std::wstring& path_xml);
    void add_mod(const Mod& mod);
    void delete_mod(int id, Mod::Type type);
    std::vector<Mod>::iterator find_db_mod(const std::wstring& id);
    std::vector<Mod>::iterator find_sdk_mod(const std::wstring& id);
    void install_mods(int db_id, int sdk_id);
    bool exists_mods(int db_id, int sdk_id);
    std::wstring get_name_folder(int db_id, int sdk_id);
    void update_mod(int db_id, int sdk_id);
    signals:
    void change_db();
    void change_sdk();
};


#endif //MAGICBOXREDUXCMAKE_MODCONTROLLER_H
