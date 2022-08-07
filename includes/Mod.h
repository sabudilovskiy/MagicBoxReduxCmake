#ifndef MOD_H
#define MOD_H

#include <string>
#include <vector>
#include "xml.h"
class Mod
{
public:
    enum Type : bool{
        DB, SDK
    };
protected:
    std::wstring _id, _name;
    std::vector<std::wstring> _verisons;
    Type _type;
public:
    Mod(std::wstring id, XMLNode& node);
    const std::wstring& get_id() const;
    const std::wstring& get_name() const;
    const std::vector<std::wstring>& get_versions() const;
    void add_version(const std::wstring& version);
    const std::wstring& get_version();
    Type get_type() const;
};

#endif // MOD_H
