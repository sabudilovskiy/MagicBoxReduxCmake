#include "xmlnode.h"
#include <algorithm>
#include <fstream>
XMLNode::XMLNode(std::wstring value): _value(std::move(value)), _type(XMLNodeType::VALUE){

}

XMLNode::XMLNode() : _type(XMLNodeType::NAMESPACE){

}

XMLNode::~XMLNode()
{
    for (auto& i : _childs){
        delete i.second;
    }
}

XMLNodeType XMLNode::get_type() const{
    return _type;
}

std::vector<std::pair<std::wstring, XMLNode*>>::const_iterator XMLNode::cbegin() const
{
    return _childs.cbegin();
}

std::vector<std::pair<std::wstring, XMLNode*>>::const_iterator XMLNode::cend() const
{
    return _childs.cend();
}

std::vector<std::pair<std::wstring, XMLNode*>>::iterator XMLNode::begin()
{
    return _childs.begin();
}

std::vector<std::pair<std::wstring, XMLNode*>>::iterator XMLNode::end()
{
    return _childs.end();
}
bool operator==(const std::pair<std::wstring, XMLNode*> elem, const std::wstring& key){
    return elem.first == key;
}
void XMLNode::add_child(std::wstring name, XMLNode *child){
    _childs.emplace_back(std::move(name), child);
}

XMLNode *XMLNode::add_child(std::wstring name)
{
    XMLNode* new_node = new XMLNode();
    _childs.emplace_back(std::move(name), new_node);
    return new_node;
}

XMLNode *XMLNode::add_child(std::wstring name, std::wstring value)
{
    XMLNode* new_node = new XMLNode(std::move(value));
    _childs.emplace_back(std::move(name), new_node);
    return new_node;
}

XMLNode* XMLNode::get_child(const std::wstring& name)
{
    auto found = std::find(_childs.begin(), _childs.end(), std::move(name));
    if (found != _childs.end()){
        return found->second;
    }
    else return nullptr;
}

const std::wstring &XMLNode::get_value() const
{
    return _value;
}

void XMLNode::set_value(std::wstring new_value)
{
    _value = new_value;
}

void XMLNode::load_to_file(std::wstring_view name, std::wostream &file)
{
    file << L'<';
    file << name;
    file << L'>';
    if (_type == XMLNodeType::NAMESPACE){
        file << L"\n\n";
        std::sort(_childs.begin(), _childs.end());
        for (auto& child : _childs){
            child.second->load_to_file(child.first, file);
            file <<  L"\n";
        }
    }
    else file << _value;
    file << L"</";
    file << name;
    file << L">\n";
    file.flush();
}

XMLNode const *XMLNode::get_child(const std::wstring& name) const {
    auto found = std::find(_childs.begin(), _childs.end(), name);
    if (found != _childs.end()){
        return found->second;
    }
    else return nullptr;
}

std::vector<std::pair<std::wstring, XMLNode *>>::const_iterator XMLNode::begin() const {
    return _childs.cbegin();
}
std::vector<std::pair<std::wstring, XMLNode *>>::const_iterator XMLNode::end() const {
    return _childs.cend();
}

void XMLNode::change_value(std::wstring value) {
    std::swap(_value, value);
}

void XMLNode::change_type(XMLNodeType type) {
    if (_type == VALUE && type == XMLNodeType::NAMESPACE){
        _value.clear();
    }
    _type = type;
}
