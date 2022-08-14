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
        virtual void generate_fl(XMLNode& node_elems);
        void save_to_xml(XML& xml) const;
        void load_from_xml(const XML& xml);
    };
    struct EnumSetting : public Setting{
        std::vector<std::wstring> available_values;
        EnumSetting(const std::wstring &title, const std::wstring &tag, const std::wstring &type, const std::wstring &value,
                     const std::wstring &tab, std::vector<std::wstring> available_values);

        void generate_fl(XMLNode &node_elems) override;
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
    signals:
    void change_settings();
public slots:
    void load();
};


#endif //MAGICBOXREDUXCMAKE_APPSETTINGS_H
