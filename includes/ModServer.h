#ifndef MODSERVER_H
#define MODSERVER_H
#include "Mod.h"
#include "ModExceptions.h"
#include <QString>
class ModServer : public Mod
{
protected:
    std::vector<std::wstring> _urls;
    std::vector<bool> _installed;
public:
    ModServer(const std::wstring &id, const std::wstring &name, const std::vector<std::wstring> &verisons, const std::vector<std::wstring>& url, Type type);
    ModServer(const std::wstring& id, XMLNode& node);
    const std::vector<std::wstring>& get_urls() const;
    std::vector<bool> &get_installed();
    const std::vector<bool> & get_installed() const;
};

#endif // MODSERVER_H
