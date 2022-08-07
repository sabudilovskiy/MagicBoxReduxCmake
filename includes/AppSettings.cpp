//
// Created by MRV on 8/7/2022.
//

#include "AppSettings.h"

 std::vector<AppSettings::Setting>::const_iterator AppSettings::get_setting(const std::wstring &tag) const {
    auto found = std::find_if(settings.begin(),  settings.end(), [&](const Setting& setting){
        return setting.tag == tag;
    });
     return found;
}

AppSettings::AppSettings() {
    settings.emplace_back(L"path_game", L"Path", L"", L"main_tab");
    settings.emplace_back(L"path_config_ModDownloader", L"Path", L"ModDownloader.ini",  L"main_tab");
    settings.emplace_back(L"path_config_ModController", L"Path", L"ModController.ini", L"main_tab");
    settings.emplace_back(L"url_config_ModDownloader", L"String", L"", L"main_tab");
    tabs.emplace_back(L"main_tab", "Основные настройки");
    load();
}

void AppSettings::load() {
    std::wifstream file("settings.ini");
    if (file.is_open()){
        file.imbue(std::locale("ru_RU.UTF-8"));
        XML xml(file);
        for (auto& i : settings){
            auto node = xml.get_tag(i.tag);
            if (node && node->get_type() == XMLNodeType::NAMESPACE){
                auto node_value = node->get_child(L"value");
                i.value = node_value->get_value();
            }
        }
    }
}

void AppSettings::save() {
    std::wofstream file("settings.ini");
    if (file.is_open()){
        file.imbue(std::locale("ru_RU.UTF-8"));
        XML xml;
        for (auto& i : settings){
            auto node = xml.add_child(i.tag);
            node->change_type(XMLNodeType::VALUE);
            node->set_value(i.value);
        }
        xml.load_to_file(file);
    }
}

void AppSettings::generate_fl_file() {
    std::wofstream file("settings.fl");
    if (file.is_open()){
        file.imbue(std::locale("ru_RU.UTF-8"));
        XML xml;
        auto node_tabs = xml.add_tag(L"tabs");
        for (auto& i : tabs){
            auto node = node_tabs->add_child(i.id);
            node->add_child(L"title", i.title);
        }
        auto node_elems = xml.add_tag(L"elems");
        for (auto& i : settings){
            auto node = node_elems->add_child(i.tag);
            node->add_child(L"type", i.type);
            node->add_child(L"default", i.value);
            node->add_child(L"tab", i.tab);
        }
        xml.load_to_file(file);
    }
}

AppSettings::~AppSettings() {
    save();
}
