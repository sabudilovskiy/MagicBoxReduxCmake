#ifndef XMLNODE_H
#define XMLNODE_H
#include<unordered_map>
enum XMLNodeType{
    VALUE, NAMESPACE
};
class XMLNode
{
    std::vector<std::pair<std::wstring, XMLNode*>> _childs;
    std::wstring _value;
    XMLNodeType _type;
public:
    XMLNode(std::wstring value);
    XMLNode();
    ~XMLNode();
    [[nodiscard]] XMLNodeType get_type() const;
    void change_type(XMLNodeType type);
    void change_value(std::wstring value);
    [[nodiscard]] std::vector<std::pair<std::wstring, XMLNode*>>::const_iterator cbegin() const;
    [[nodiscard]] std::vector<std::pair<std::wstring, XMLNode*>>::const_iterator cend() const;
    std::vector<std::pair<std::wstring, XMLNode*>>::iterator begin();
    [[nodiscard]] std::vector<std::pair<std::wstring, XMLNode*>>::const_iterator begin() const;
    std::vector<std::pair<std::wstring, XMLNode*>>::iterator end();
    [[nodiscard]] std::vector<std::pair<std::wstring, XMLNode*>>::const_iterator end() const;
    void add_child(std::wstring name, XMLNode* child);
    XMLNode* add_child(std::wstring name);
    XMLNode* add_child(std::wstring name, std::wstring value);
    XMLNode* get_child(const std::wstring& name);
    [[nodiscard]] XMLNode const* get_child(const std::wstring& name) const;
    [[nodiscard]] const std::wstring& get_value() const;
    void set_value(std::wstring new_value);
    void load_to_file(std::wstring_view name, std::wostream& file);
};
#endif // XMLNODE_H
