#ifndef MODSERVER_H
#define MODSERVER_H
#include "Mod.h"
#include "ModExceptions.h"
#include <QString>
class ModServer : public Mod
{
protected:
    std::vector<std::wstring> _urls;
public:
    ModServer(const std::wstring& id, XMLNode& node);
    const std::vector<std::wstring>& get_urls() const;
};

#endif // MODSERVER_H
