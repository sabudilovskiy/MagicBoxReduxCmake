#include "Mod.h"
#include "ModExceptions.h"
#include <QString>
Mod::Mod(std::wstring id, XMLNode &node) : _id(std::move(id))
{
    auto node_name = node.get_child(L"name");
    if (!node_name) throw MissingName(QString::fromStdWString(_id).toStdString());
    _name = node_name->get_value();
    auto node_type = node.get_child(L"type");
    if (!node_type) throw MissingModType(QString::fromStdWString(_id).toStdString());
    std::wstring wtype = node_type->get_value();
    if (wtype == L"DB"){
        _type = Type::DB;
    }
    else if (wtype == L"SDK"){
        _type = Type::SDK;
    }
    else throw UnknownModType(QString::fromStdWString(_id).toStdString());
    auto node_versions = node.get_child(L"versions");
    if (!node_versions) throw MissingVersions(QString::fromStdWString(_id).toStdString());
    for (auto& version : *node_versions){
        _verisons.push_back(version.second->get_value());
    }
    if (_verisons.empty()){
        throw MissingVersions(QString::fromStdWString(_id).toStdString());
    }
}

const std::wstring &Mod::get_id() const
{
    return _id;
}

const std::wstring &Mod::get_name() const
{
    return _name;
}

const std::vector<std::wstring> &Mod::get_versions() const
{
    return _verisons;
}



Mod::Type Mod::get_type() const{
    return _type;
}

void Mod::add_version(const std::wstring& version) {
    auto found = std::find(_verisons.begin(),  _verisons.end(), version);
    if (found == _verisons.end()){
        _verisons.push_back(version);
    }
}

Mod::Mod(const std::wstring &id, const std::wstring &name, const std::vector<std::wstring> &verisons, Mod::Type type)
        : _id(id), _name(name), _verisons(verisons), _type(type) {}

ModVersion::ModVersion(int id_mod, int id_version) : id_mod(id_mod), id_version(id_version) {}

ModVersion::ModVersion() {
}

bool ModVersion::operator<(ModVersion other) const {
    return id_mod < other.id_mod || id_mod == other.id_mod && id_version < other.id_version;
}

bool ModVersion::operator>(ModVersion other) const {
    return other < *this;
}

bool ModVersion::operator==(ModVersion other) const {
    return id_mod==other.id_mod && id_version == other.id_version;
}

