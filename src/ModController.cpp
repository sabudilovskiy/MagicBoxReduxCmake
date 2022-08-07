//
// Created by MRV on 8/5/2022.
//

#include "ModController.h"
#include "Support.h"
#include <filesystem>
#include <sstream>
namespace fs = std::filesystem;
void ModController::install_mods(int db_id, int sdk_id) {
    try{
        std::wstring path = this->path_game + L"\\MODS\\" + get_name_folder(db_id, sdk_id);
        std::wstring path_db = L"C:\\MagicBoxRedux\\mods\\db\\" + mods_db[db_id].get_id() + L"\\" + mods_db[db_id].get_version();
        std::wstring path_sdk = L"C:\\MagicBoxRedux\\mods\\sdk\\" + mods_sdk[sdk_id].get_id() + L"\\" + mods_sdk[sdk_id].get_version();
        fs::copy(path_db, path, fs::copy_options::overwrite_existing|fs::copy_options::recursive);
        fs::copy(path_sdk, path, fs::copy_options::overwrite_existing|fs::copy_options::recursive);
    }
    catch (const fs::filesystem_error& ex){

    }
}

bool ModController::exists_mods(int db_id, int sdk_id) {
    std::wstring path = this->path_game + L"\\MODS\\" + get_name_folder(db_id, sdk_id);
    return fs::exists(path);
}

std::wstring ModController::get_name_folder(int db_id, int sdk_id) {
    return mods_db[db_id].get_id() + L'_' + mods_db[db_id].get_version() + L'_' + mods_sdk[sdk_id].get_id() + L'_' + mods_sdk[sdk_id].get_version();
}

void ModController::update_mod(int db_id, int sdk_id) {
    std::wstring name_folder = get_name_folder(db_id, sdk_id);
    std::wstring wpath(path_game + L"\\joshua.cfg");
    if (!std::filesystem::exists(wpath)){
        return;
    }
    std::ifstream settings(wpath, std::ios::in|std::ios::binary);
    if (!settings.is_open()){
        return;
    }
    std::wstring ws = utf_16_bom_to_utf_8(settings);
    std::wstringstream ss(ws);
    XML settings_xml(ss);
    settings_xml.get_child(L"MAIN_CONFIG")->get_child(L"MOD")->set_value(name_folder);
    settings.close();
    std::wstringstream settings_new_ss(wpath);
    settings.imbue(std::locale("ru_RU.UTF-8"));
    settings_xml.load_to_file(settings_new_ss);
    std::ofstream settings_new(wpath, std::ios::out|std::ios::binary);
    utf_8_to_utf_16_bom(settings_new_ss.str(), settings_new);
}

ModController::ModController(const std::wstring &path_game, const XML &xml) :path_game(path_game){
    load_from_xml(xml);
}

void ModController::load_from_xml(const XML& xml) {
    auto mods_node = xml.get_child(L"mods");
    if (mods_node){
        for (const auto& cur_mod : *mods_node){
            Mod mod(cur_mod.first, *cur_mod.second);
            if (mod.get_type() == Mod::DB){
                mods_db.push_back(std::move(mod));
            }
            else {
                mods_sdk.push_back(std::move(mod));
            }
        }
    }
}

void ModController::add_mod(const Mod &mod) {
    std::vector<Mod>::iterator found;
    if (mod.get_type() == Mod::DB){
        found = find_db_mod(found->get_id());
        if (found!=mods_db.end()){
            for (auto& i : mod.get_versions()) found->add_version(i);
        }
        else mods_db.push_back(mod);
        emit change_db();
    }
    else {
        found = find_sdk_mod(found->get_id());
        if (found!=mods_sdk.end()){
            for (auto& i : mod.get_versions()) found->add_version(i);
        }
        else mods_sdk.push_back(mod);
        emit change_sdk();
    }
}

ModController::ModController(const std::wstring &path_game, const std::wstring &path_xml) : path_game(path_game), path_xml(path_xml){
    std::wifstream file(path_xml);
    file.imbue(std::locale("ru_RU.UTF-8"));
    if (file.is_open()){
        XML xml(file);
        load_from_xml(xml);
    }
}

void ModController::delete_mod(int id, Mod::Type type) {
    if (type == Mod::Type::DB){
        mods_db.erase(mods_db.begin() + id);
        emit change_db();
    }
    else {
        mods_sdk.erase(mods_sdk.begin() + id);
        emit change_sdk();
    }
}

std::vector<Mod>::iterator ModController::find_db_mod(const std::wstring &id) {
    return std::find_if(mods_db.begin(),  mods_db.end(), [&id](const Mod& mod){
        return mod.get_id() == id;
    });
}

std::vector<Mod>::iterator ModController::find_sdk_mod(const std::wstring &id) {
    return std::find_if(mods_sdk.begin(),  mods_sdk.end(), [&id](const Mod& mod){
        return mod.get_id() == id;
    });
}
