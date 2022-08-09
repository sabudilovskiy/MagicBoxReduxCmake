//
// Created by MRV on 8/7/2022.
//

#ifndef MAGICBOXREDUXCMAKE_APPSETTINGS_H
#define MAGICBOXREDUXCMAKE_APPSETTINGS_H
#include <vector>
#include <string>
#include <fstream>
#include <QString>
#include <QList>
#include <QObject>
#include "xml.h"

class AppSettings : public QObject{
public:
    struct Setting{
        std::wstring title;
        std::wstring tag;
        std::wstring type;
        std::wstring value;
        std::wstring tab;
        Setting(const std::wstring &title, const std::wstring &tag, const std::wstring &type, const std::wstring &value,
                const std::wstring &tab);
        virtual void generate_fl(XMLNode& node_elems){
            auto node = node_elems.add_child(tag);
            node->add_child(L"type", type);
            node->add_child(L"default", value);
            node->add_child(L"tab", tab);
            node->add_child(L"title", title);
        }
        void save_to_xml(XML& xml) const{
            auto node = xml.add_tag(tag);
            node->change_type(XMLNodeType::VALUE);
            node->set_value(value);
        }
        void load_from_xml(const XML& xml){
            auto node = xml.get_tag(tag);
            if (node && node->get_type() == XMLNodeType::VALUE){
                value = node->get_value();
            }
        }
    };
    struct EnumSetting : public Setting{
        std::vector<std::wstring> available_values;
        EnumSetting(const std::wstring &title, const std::wstring &tag, const std::wstring &type, const std::wstring &value,
                     const std::wstring &tab, std::vector<std::wstring> available_values) : Setting(title, tag, type, value, tab), available_values(available_values){}

        void generate_fl(XMLNode &node_elems) override {
            auto node = node_elems.add_child(tag);
            node->add_child(L"type", type);
            node->add_child(L"default", value);
            node->add_child(L"tab", tab);
            node->add_child(L"title", title);
            auto node_availableValues = node->add_child(L"availableValues");
            int i = 0;
            for (auto & availableValue : available_values){
                node_availableValues->add_child(std::to_wstring(i), availableValue);
                ++i;
            }
        }
    };
    struct Tab{
        std::wstring id;
        std::wstring title;
        Tab(const std::wstring &id, const std::wstring &title);
    };
protected:
    Q_OBJECT
    std::vector<Tab> tabs;
    std::vector<Setting*> settings;
public:
    AppSettings();
    void save();
    void generate_fl_file();
    [[nodiscard]] const std::wstring& get_setting(const std::wstring& tag) const;
    ~AppSettings() override;
public slots:
    void load();
};


#endif //MAGICBOXREDUXCMAKE_APPSETTINGS_H
