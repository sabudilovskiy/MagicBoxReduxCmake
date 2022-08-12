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
                                          L"policies_downloadInstalledMods",
                                          L"Enum",
                                          L"ASK",
                                          L"downloader_tab",
                                          {L"SKIP", L"REINSTALL", L"ASK"}));
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
        auto node_files = xml.add_tag(L"files");
        node_files->add_child(L"1", L"settings.ini");
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

void AppSettings::Setting::generate_fl(XMLNode &node_elems) {
    auto node = node_elems.add_child(tag);
    node->add_child(L"type", type);
    node->add_child(L"default", value);
    node->add_child(L"tab", tab);
    node->add_child(L"title", title);
    node->add_child(L"file", L"settings.ini");
}

void AppSettings::Setting::save_to_xml(XML &xml) const {
    auto node = xml.add_tag(tag);
    node->change_type(XMLNodeType::VALUE);
    node->set_value(value);
}

void AppSettings::Setting::load_from_xml(const XML &xml) {
    auto node = xml.get_tag(tag);
    if (node && node->get_type() == XMLNodeType::VALUE){
        value = node->get_value();
    }
}

AppSettings::Tab::Tab(const std::wstring &id, const std::wstring &title) : id(id), title(title) {}

void AppSettings::EnumSetting::generate_fl(XMLNode &node_elems) {
    auto node = node_elems.add_child(tag);
    node->add_child(L"type", type);
    node->add_child(L"default", value);
    node->add_child(L"tab", tab);
    node->add_child(L"title", title);
    node->add_child(L"file", L"settings.ini");
    auto node_availableValues = node->add_child(L"availableValues");
    int i = 0;
    for (auto & availableValue : available_values){
        node_availableValues->add_child(std::to_wstring(i), availableValue);
        ++i;
    }
}

AppSettings::EnumSetting::EnumSetting(const std::wstring &title, const std::wstring &tag, const std::wstring &type,
                                      const std::wstring &value, const std::wstring &tab,
                                      std::vector<std::wstring> available_values) : Setting(title, tag, type, value, tab), available_values(available_values){}
