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
#include "xml.h"

class AppSettings {
public:
    struct Setting{
        std::wstring tag;
        std::wstring type;
        std::wstring value;
        std::wstring tab;
    };
    struct Tab{
        std::wstring id;
        std::wstring title;
    };
protected:
    std::vector<Tab> tabs;
    std::vector<Setting> settings;
public:
    AppSettings();
    void load();
    void save();
    void generate_fl_file();
    [[nodiscard]] std::vector<AppSettings::Setting>::const_iterator get_setting(const std::wstring& tag) const;
    ~AppSettings();
};


#endif //MAGICBOXREDUXCMAKE_APPSETTINGS_H
