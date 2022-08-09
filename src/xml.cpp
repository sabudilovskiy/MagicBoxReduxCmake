#include "xml.h"
#include <QString>
#include <queue>
#include <stack>
#include <QList>

class XMLItem{
public:
    enum XMLType{
        VALUE, OPEN_TAG, CLOSE_TAG
    };
protected:
    XMLType type;
    std::wstring info;
public:
    XMLItem(XMLType type, std::wstring info) : type(type), info(std::move(info)) {}
    XMLType get_type() const{
        return type;
    }
    const std::wstring &get_info() const {
        return info;
    }
};
std::vector<XMLItem> parsefile(std::wistream &file){
    std::vector<XMLItem> xml;
    while (!file.eof()){
        std::wstring temp;
        wchar_t begin = file.get();
        if (begin == '<'){
            wchar_t cur = file.get();
            while (cur != '>'){
                temp.push_back(cur);
                cur = file.get();
            }
            if (temp[0] == '/'){
                temp.erase(temp.begin());
                xml.emplace_back(XMLItem::XMLType::CLOSE_TAG, std::move(temp));
            }
            else if (temp[0] == '!'){
                int last = temp.size() - 1;
                if (temp.size() < 5 || temp[1] != '-' || temp[2] != '-' || temp[last - 1] != '-' || temp[last] != '-') {
                    xml.emplace_back(XMLItem::XMLType::OPEN_TAG, std::move(temp));
                }
            }
            else{
                xml.emplace_back(XMLItem::XMLType::OPEN_TAG, std::move(temp));
            }
        }
        else if (begin != '\n' && begin != ' ' && begin != 65535 && begin != '\r'){
            temp.push_back(begin);
            wchar_t cur = file.peek();
            while (cur != '<' && !file.eof()){
                file.get();
                temp.push_back(cur);
                cur = file.peek();
            }
            xml.emplace_back(XMLItem::XMLType::VALUE, std::move(temp));
        }
    }
    return xml;
}

XMLNode* node_from_list(std::vector<XMLItem>& items, int begin = 0, int end = -1){
    if (end == -1){
        end = items.size();
    }
    int i = begin;
    int n = end;
    if (items[i].get_type() == XMLItem::XMLType::VALUE && end - begin == 1){
        return new XMLNode(items[i].get_info());
    }
    XMLNode* answer = new XMLNode;
    while (i < n){
        int begin_node = i + 1;
        if (items[i].get_type() != XMLItem::XMLType::OPEN_TAG){
            throw InCorrectXML();
        }
        std::wstring name = items[i].get_info();
        i++;
        while (i < n && !(items[i].get_type()==XMLItem::XMLType::CLOSE_TAG && items[i].get_info() == name)){
            i++;
        }
        if (i >= n || items[i].get_type() != XMLItem::XMLType::CLOSE_TAG){
            throw UnclosedTag(QString::fromStdWString(name).toStdString());
        }
        int end_node = i;
        answer->add_child(name, node_from_list(items, begin_node, end_node));
        i++;
    }
    return answer;
}
XML::XML()
{
    root = new XMLNode();
}

XML::XML(std::wistream &file)
{
    std::vector<XMLItem> items = parsefile(file);
    if (items.size() > 0){
        root = node_from_list(items);
    }
    else root = new XMLNode(L"");
}

void XML::save_to_file(std::wostream &file)
{
    for (auto& child : *root){
        child.second->save_to_file(child.first, file);
    }
}

XML::~XML()
{
    delete root;
}

XMLNode* XML::get_child(const std::wstring &name)
{
    return root->get_child(name);
}
XMLNode const*  XML::get_child(const std::wstring &name) const
{
    return root->get_child(name);
}
std::vector<std::pair<std::wstring, XMLNode*>>::const_iterator XML::cbegin() const
{
    return root->cbegin();
}

std::vector<std::pair<std::wstring, XMLNode*>>::const_iterator XML::cend() const
{
    return root->cend();
}

std::vector<std::pair<std::wstring, XMLNode*>>::iterator XML::begin()
{
    return root->begin();
}

std::vector<std::pair<std::wstring, XMLNode*>>::iterator XML::end()
{
    return root->end();
}

XMLNode *XML::get_root(){
    return root;
}

XMLNode *XML::get_tag(const std::wstring &tag) {
    auto tags = QString::fromStdWString(tag).split('_');
    auto node = root;
    auto it = tags.begin();
    auto end = tags.end();
    while (!node && it!=end){
        node = root->get_child(it->toStdWString());
        ++it;
    }
    return node;
}

XMLNode const *XML::get_tag(const std::wstring &tag) const {
    auto tags = QString::fromStdWString(tag).split('_');
    auto node = root;
    auto it = tags.begin();
    auto end = tags.end();
    while (!node && it!=end){
        node = root->get_child(it->toStdWString());
        ++it;
    }
    return node;
}

XMLNode *XML::add_tag(const std::wstring &tag) {
    auto tags = QString::fromStdWString(tag).split('_');
    auto node = root;
    auto it = tags.begin();
    auto end = tags.end();
    auto old_node = node;
    while (it!=end){
        std::wstring wtag = it->toStdWString();
        old_node = node;
        node = node->get_child(wtag);
        if (!node){
            node = old_node->add_child(wtag);
        }
        ++it;
    }
    return node;
}

XMLNode *XML::add_child(std::wstring name) {
    return root->add_child(std::move(name));
}

