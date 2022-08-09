//
// Created by MRV on 8/5/2022.
//

#ifndef MAGICBOXREDUXCMAKE_MODCONTROLLER_H
#define MAGICBOXREDUXCMAKE_MODCONTROLLER_H

#include "Mod.h"
#include <vector>
#include <QString>
#include <QObject>
class ModController : public QObject {
protected:
    Q_OBJECT
    const std::wstring& path_game;
    const std::wstring& path_xml;
    std::vector<Mod> mods_db;
    std::vector<Mod> mods_sdk;
    int count_db = 0;
    int count_sdk = 0;
    void load_from_xml(const XML& xml);
    void save_to_xml(XML& xml);
public:
    ModController(const std::wstring& path_game, const std::wstring& path_xml);
    ~ModController() override;
    void add_mod(const Mod& mod);
    void delete_mod(int id, Mod::Type type);
    std::vector<Mod>::iterator find_db_mod(const std::wstring& id);
    std::vector<Mod>::iterator find_sdk_mod(const std::wstring& id);
    void install_mods(ModVersion db, ModVersion sdk);
    bool exists_mods(ModVersion db, ModVersion sdk);
    std::wstring get_name_folder(ModVersion db, ModVersion sdk);
    void update_mod(ModVersion db, ModVersion sdk);
    [[nodiscard]] int get_count_db() const;
    [[nodiscard]] int get_count_sdk() const;
    [[nodiscard]] const std::vector<Mod> &get_mods_db() const;
    [[nodiscard]] const std::vector<Mod> &get_mods_sdk() const;

signals:
    void change_db();
    void change_sdk();
};


#endif //MAGICBOXREDUXCMAKE_MODCONTROLLER_H
