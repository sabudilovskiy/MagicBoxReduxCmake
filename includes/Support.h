#ifndef SUPPORT_H
#define SUPPORT_H

#include <string>
#include <QString>
#include <stdexcept>
class ToDo : public std::exception{};
void unpack_mod_zip(const std::string& path_zip, const std::string& path);
std::string string_from_wstring(const std::wstring& wstring);
std::wstring wstring_from_string(const std::string& string);
std::string get_folder(const std::string& path);
std::wstring get_folder(const std::wstring& path);
std::wstring utf_16_bom_to_utf_8(std::ifstream& file);
void utf_8_to_utf_16_bom(const std::wstring& str, std::ofstream& file);
template<typename Container>
bool include(const Container& container, typename Container::const_reference elem) {
    return std::find(container.begin(), container.end(), elem) != container.end();
}

#endif // SUPPORT_H
