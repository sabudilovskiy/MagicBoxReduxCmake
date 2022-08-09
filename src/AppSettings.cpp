//
// Created by MRV on 8/7/2022.
//

#include "AppSettings.h"

 const std::wstring& AppSettings::get_setting(const std::wstring &tag) const {
    auto found = std::find_if(settings.begin(),  settings.end(), [&](const Setting* const setting){
        return setting->tag == tag;
    });
    return (*found)->value;
}

AppSettings::AppSettings() {
    settings.emplace_back(new Setting(L"Путь к игре",
                                      L"path_game",
                                      L"PathFolder",
                                      L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\SuperPower 2",
                                      L"main_tab"));
    settings.emplace_back(new Setting(L"Путь к конфигу загрузчика модов",
                                      L"path_config_ModDownloader",
                                      L"PathFile",
                                      L"C:\\MagicBoxReduxRelease\\ModDownloader.ini",
                                      L"downloader_tab"));
    settings.emplace_back(new Setting(L"Путь к конфигу менеджера модов",
                                      L"path_config_ModController",
                                      L"PathFile",
                                      L"C:\\MagicBoxReduxRelease\\ModController.ini",
                                      L"main_tab"));
    settings.emplace_back(new Setting(L"Ссылка на конфиг загрузчика модов",
                                      L"url_config_ModDownloader",
                                      L"String",
                                      L"https://www.dropbox.com/s/3k8tf3r1d1o2s8z/ModDownloader.ini?dl=1",
                                      L"downloader_tab"));
    settings.emplace_back(new EnumSetting(L"Скачивание установленных модов",
                                          L"",
                                          L"",
                                          L"",
                                          L"",
                                          {}
            ));
    tabs.emplace_back(L"main_tab",
                      L"Основные настройки");
    tabs.emplace_back(L"downloader_tab",
                      L"Настройки загрузчика");
    load();
}

void AppSettings::load() {
    std::wifstream file(L"settings.ini");
    if (file.is_open()){
        file.imbue(std::locale("ru_RU.UTF-8"));
        XML xml(file);
        for (auto i : settings){
            i->load_from_xml(xml);
        }
    }
}

void AppSettings::save() {
    std::wofstream file(L"settings.ini");
    if (file.is_open()){
        file.imbue(std::locale("ru_RU.UTF-8"));
        XML xml;
        for (auto i : settings){
            i->save_to_xml(xml);
        }
        xml.save_to_file(file);
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
            i->generate_fl(*node_elems);
        }
        xml.save_to_file(file);
    }
}

AppSettings::~AppSettings() {
    save();
}

AppSettings::Setting::Setting(const std::wstring &title, const std::wstring &tag, const std::wstring &type,
                              const std::wstring &value,
                              const std::wstring &tab) : title(title), tag(tag), type(type), value(value), tab(tab) {}

AppSettings::Tab::Tab(const std::wstring &id, const std::wstring &title) : id(id), title(title) {}
