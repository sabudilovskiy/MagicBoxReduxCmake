#ifndef XML_H
#define XML_H

#include "xmlitem.h"
#include <utility>
#include <vector>
#include <fstream>
#include <xmlnode.h>
#include <algorithm>
#include <stdexcept>
class XML{
    XMLNode* root;
public:
    XML();
    XML(std::wistream& file);
    void load_to_file(std::wostream& file);
    XMLNode* add_tag(const std::wstring& tag);
    XMLNode* add_child(std::wstring name);
    [[nodiscard]]  XMLNode* get_tag(const std::wstring& tag);
    [[nodiscard]] XMLNode const* get_tag(const std::wstring& tag) const;
    [[nodiscard]]  XMLNode* get_child(const std::wstring& name);
    [[nodiscard]] XMLNode const* get_child(const std::wstring& name) const;
    [[nodiscard]]  XMLNode* get_root();
    [[nodiscard]]  std::vector<std::pair<std::wstring, XMLNode*>>::const_iterator cbegin() const;
    [[nodiscard]] std::vector<std::pair<std::wstring, XMLNode*>>::const_iterator cend() const;
    [[nodiscard]] std::vector<std::pair<std::wstring, XMLNode*>>::iterator begin();
    [[nodiscard]]  std::vector<std::pair<std::wstring, XMLNode*>>::iterator end();
    ~XML();
};

class UnclosedTag : public std::exception{
    std::string name_tag;
public:
    const char * what () const override{
        return name_tag.c_str();
    }
    UnclosedTag(std::string name_tag):name_tag(name_tag + " не закрыт тег"){
    }
};

class InCorrectXML : public std::exception{

};
#endif // XML_H
