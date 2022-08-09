#ifndef MOD_H
#define MOD_H

#include <string>
#include <vector>
#include "xml.h"
struct ModVersion{
    int id_mod;
    int id_version;
    ModVersion();
    ModVersion(int id_mod, int id_version);
    bool operator<(ModVersion other) const;
    bool operator>(ModVersion other) const;
    bool operator==(ModVersion other) const;
};
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
    Mod(const std::wstring &id, const std::wstring &name, const std::vector<std::wstring> &verisons, Type type);
    const std::wstring& get_id() const;
    const std::wstring& get_name() const;
    const std::vector<std::wstring>& get_versions() const;
    void add_version(const std::wstring& version);
    Type get_type() const;
    const bool operator>(const Mod& right) const{
        return _id > right._id;
    }
    bool operator<(const Mod& right) const{
        return right > *this;
    }
};

#endif // MOD_H
