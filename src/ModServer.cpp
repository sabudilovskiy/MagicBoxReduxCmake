#include "ModServer.h"


ModServer::ModServer(const std::wstring &id, XMLNode &node) : Mod(id, node){
    auto node_versions = node.get_child(L"versions");
    for (auto& version : *node_versions){
        auto node_version_url = version.second->get_child(L"url");
        if (!node_version_url) throw MissingVersionName(QString::fromStdWString(_id).toStdString());
        _urls.push_back(node_version_url->get_value());
    }
}

const std::vector<std::wstring> &ModServer::get_urls() const
{
    return _urls;
}
