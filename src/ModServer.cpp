#include "ModServer.h"


ModServer::ModServer(const std::wstring &id, XMLNode &node) : Mod(id, node){
    _installed.resize(_verisons.size(), false);
    auto node_urls = node.get_child(L"urls");
    if (!node_urls) throw MissingVersionURL(QString::fromStdWString(_id).toStdString());
    for (auto& url : *node_urls)_urls.push_back(url.second->get_value());
    if (_urls.size() != _verisons.size()) throw MissingVersionURL(QString::fromStdWString(_id).toStdString());
}

const std::vector<std::wstring> &ModServer::get_urls() const
{
    return _urls;
}

std::vector<bool> &ModServer::get_installed() {
    return _installed;
}

ModServer::ModServer(const std::wstring &id, const std::wstring &name, const std::vector<std::wstring> &verisons,
                     const std::vector<std::wstring> &url, Mod::Type type) :
        Mod(id, name, verisons, type), _urls(url){
}

const std::vector<bool> &ModServer::get_installed() const {
    return _installed;
}
