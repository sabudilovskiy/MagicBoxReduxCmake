//
// Created by MRV on 8/6/2022.
//
#include "Support.h"
#include <QString>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <libzippp/libzippp.h>
std::wstring wstring_from_string(const std::string &string) {
    return QString::fromStdString(string).toStdWString();
}

std::string string_from_wstring(const std::wstring &wstring) {
    return QString::fromStdWString(wstring).toStdString();
}

void unpack_mod_zip(const std::string &path_zip, const std::string &path) {
    libzippp::ZipArchive archive(path_zip);
    archive.open(libzippp::ZipArchive::ReadOnly);
    if (archive.isOpen()){
        if (!std::filesystem::exists(path)){
            std::filesystem::create_directories(path);
        }
        auto entryies = archive.getEntries();
        for (auto& file : entryies){
            std::string name = file.getName();
            if (name.length() > 4 && name.substr(0, 4) == "mod/"){
                //Обрезаем mod/
                std::string short_name = name.substr(4);
                if (file.isDirectory()){
                    std::string full_path = path;
                    full_path += "/";
                    full_path += short_name;
                    if (!std::filesystem::exists(full_path)){
                        std::filesystem::create_directories(full_path);
                    }
                }
                else {
                    std::string cur_path = path;
                    cur_path += '/';
                    cur_path += short_name;
                    std::ofstream unarchived_file(cur_path, std::ios::binary);
                    file.readContent(unarchived_file);
                    unarchived_file.close();
                }
            }
        }
    }
}

std::string get_folder(const std::string &path){
    int i = path.length() - 1;
    while (i > -1 && path[i] != '/' && path[i] != '\\'){
        i--;
    }
    return path.substr(0,i);
}

std::wstring get_folder(const std::wstring &path){
    int i = path.length() - 1;
    while (i > -1 && path[i] != '/' && path[i] != '\\'){
        i--;
    }
    return path.substr(0,i);
}

std::wstring utf_16_bom_to_utf_8(std::ifstream &file) {
    std::stringstream ss;
    ss << file.rdbuf();
    std::wstring ws;
    ws.resize(ss.str().size() / sizeof (wchar_t ) - 1);
    std::memcpy(&ws[0], &ss.str()[2], ss.str().size() - 2);
    return ws;
}

void utf_8_to_utf_16_bom(const std::wstring &str, std::ofstream &file) {
    wchar_t bom = -257;
    size_t wsize = sizeof(wchar_t);
    file.write((char*)(&bom), wsize);
    file.write((char*)str.c_str(), str.size() * wsize);
    file.close();
}
