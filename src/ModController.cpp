//
// Created by MRV on 8/5/2022.
//

#include "ModController.h"
#include "Support.h"
#include <filesystem>
#include <sstream>
namespace fs = std::filesystem;
void ModController::install_mods(ModVersion db, ModVersion sdk) {
    try{
        std::wstring path = this->path_game + L"\\MODS\\" + get_name_folder(db, sdk);
        std::wstring path_db = L"C:\\MagicBoxReduxRelease\\mods\\db\\" + mods_db[db.id_mod].get_id() + L"\\" + mods_db[db.id_mod].get_versions()[db.id_version];
        std::wstring path_sdk = L"C:\\MagicBoxReduxRelease\\mods\\sdk\\" + mods_sdk[sdk.id_mod].get_id() + L"\\" + mods_sdk[sdk.id_mod].get_versions()[db.id_version];
        if (!std::filesystem::exists(path)){
            std::filesystem::create_directories(path);
        }
        fs::copy(path_db, path, fs::copy_options::overwrite_existing|fs::copy_options::recursive);
        fs::copy(path_sdk, path, fs::copy_options::overwrite_existing|fs::copy_options::recursive);
    }
    catch (const fs::filesystem_error& ex){

    }
}

bool ModController::exists_mods(ModVersion db, ModVersion sdk) {
    std::wstring path = this->path_game + L"\\MODS\\" + get_name_folder(db, sdk);
    return fs::exists(path);
}

std::wstring ModController::get_name_folder(ModVersion db, ModVersion sdk) {
    return mods_db[db.id_mod].get_id() + L'_' + mods_db[db.id_mod].get_versions()[db.id_version]
    + L'_' + mods_sdk[sdk.id_mod].get_id() + L'_' + mods_sdk[sdk.id_mod].get_versions()[sdk.id_version];
}

void ModController::update_mod(ModVersion db, ModVersion sdk) {
    std::wstring name_folder = get_name_folder(db, sdk);
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
    settings_xml.save_to_file(settings_new_ss);
    std::ofstream settings_new(wpath, std::ios::out|std::ios::binary);
    utf_8_to_utf_16_bom(settings_new_ss.str(), settings_new);
}

void ModController::add_mod(const Mod &mod) {
    std::vector<Mod>::iterator found;
    if (mod.get_type() == Mod::DB){
        found = find_db_mod(mod.get_id());
        if (found!=mods_db.end()){
            for (auto& i : mod.get_versions()) {
                found->add_version(i);
                count_db++;
            }
        }
        else mods_db.push_back(mod);
        emit change_db();
    }
    else {
        found = find_sdk_mod(mod.get_id());
        if (found!=mods_sdk.end()){
            for (auto& i : mod.get_versions()) {
                found->add_version(i);
                count_sdk++;
            }
        }
        else mods_sdk.push_back(mod);
        emit change_sdk();
    }
}

void ModController::delete_mod(int id, Mod::Type type) {
    throw ToDo();
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

int ModController::get_count_sdk() const {
    return count_sdk;
}

int ModController::get_count_db() const {
    return count_db;
}

const std::vector<Mod> &ModController::get_mods_db() const {
    return mods_db;
}

const std::vector<Mod> &ModController::get_mods_sdk() const {
    return mods_sdk;
}
ModController::ModController(const std::wstring &path_game, const std::wstring &path_xml) : path_game(path_game), path_xml(path_xml){
    std::wifstream file(path_xml);
    file.imbue(std::locale("ru_RU.UTF-8"));
    if (file.is_open()){
        XML xml(file);
        load_from_xml(xml);
    }
}

ModController::~ModController() {
    std::wofstream file(path_xml);
    file.imbue(std::locale("ru_RU.UTF-8"));
    if (file.is_open()){
        XML xml;
        save_to_xml(xml);
        xml.save_to_file(file);
    }
}

void ModController::save_to_xml( XML &xml) {
    std::sort(mods_db.begin(),  mods_db.end());
    std::sort(mods_sdk.begin(),  mods_sdk.end());
    auto node_mods = xml.add_child(L"mods");
    for (auto & mod : mods_db){
        auto mod_node = node_mods->add_child(mod.get_id());
        mod_node->add_child(L"name", mod.get_name());
        mod_node->add_child(L"type", L"DB");
        auto mod_versions = mod_node->add_child(L"versions");
        int i = 0;
        for (auto &version : mod.get_versions()){
            mod_versions->add_child(std::to_wstring(i), version);
            ++i;
        }
    }
}

void ModController::load_from_xml(const XML& xml) {
    auto mods_node = xml.get_child(L"mods");
    count_db = 0;
    count_sdk = 0;
    if (mods_node){
        for (const auto& cur_mod : *mods_node){
            Mod mod(cur_mod.first, *cur_mod.second);
            if (mod.get_type() == Mod::DB){
                count_db += mod.get_versions().size();
                mods_db.push_back(std::move(mod));
            }
            else {
                count_sdk += mod.get_versions().size();
                mods_sdk.push_back(std::move(mod));
            }
        }
    }
    std::sort(mods_db.begin(),  mods_db.end());
    std::sort(mods_sdk.begin(),  mods_sdk.end());
}