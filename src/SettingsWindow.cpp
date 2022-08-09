#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"
#include "xml.h"
#include <QScrollArea>
#include <QFormLayout>
#include <filesystem>
#include <fstream>
#include <QMessageBox>

void load_info(QSpinBox* box, XMLNode* root){
    auto paths = box->objectName().split('_');
    XMLNode* node = root;
    for (auto& path : paths){
        node = node->get_child(path.toStdWString());
    }
    box->setValue(std::stoi(node->get_value()));
}
void load_info(QDoubleSpinBox* box, XMLNode* root){
    auto paths = box->objectName().split('_');
    XMLNode* node = root;
    for (auto& path : paths){
        node = node->get_child(path.toStdWString());
    }
    box->setValue(std::stod(node->get_value()));
}
void load_info(QCheckBox* box, XMLNode* root){
    auto paths = box->objectName().split('_');
    XMLNode* node = root;
    for (auto& path : paths){
        node = node->get_child(path.toStdWString());
    }
    box->setTristate(std::stoi(node->get_value()));
}
void load_info(QTextEdit* box, XMLNode* root){
    auto paths = box->objectName().split('_');
    XMLNode* node = root;
    for (auto& path : paths){
        node = node->get_child(path.toStdWString());
    }
    box->setText(QString::fromStdWString(node->get_value()));
}
void load_info(QComboBox* box, XMLNode* root){
    auto paths = box->objectName().split('_');
    XMLNode* node = root;
    for (auto& path : paths){
        node = node->get_child(path.toStdWString());
    }
    box->setCurrentText(QString::fromStdWString(node->get_value()));
}
void store_info(QSpinBox* box, XMLNode* root){
    auto paths = box->objectName().split('_');
    XMLNode* node = root;
    for (int i = 0; i < paths.size() - 1; i++){
        auto old_node = node;
        std::wstring wpath = paths[i].toStdWString();
        node = old_node->get_child(wpath);
        if (!node){
            node = old_node->add_child(wpath);
        }
    }
    node->add_child(paths[paths.size() - 1].toStdWString(), box->text().toStdWString());
}
void store_info(QDoubleSpinBox* box, XMLNode* root){
    auto paths = box->objectName().split('_');
    XMLNode* node = root;
    for (int i = 0; i < paths.size() - 1; i++){
        auto old_node = node;
        std::wstring wpath = paths[i].toStdWString();
        node = old_node->get_child(wpath);
        if (!node){
            node = old_node->add_child(wpath);
        }
    }
    node->add_child(paths[paths.size() - 1].toStdWString(), box->text().toStdWString());
}
void store_info(QTextEdit* text_edit, XMLNode* root){
    auto paths = text_edit->objectName().split('_');
    XMLNode* node = root;
    for (int i = 0; i < paths.size() - 1; i++){
        auto old_node = node;
        std::wstring wpath = paths[i].toStdWString();
        node = old_node->get_child(wpath);
        if (!node){
            node = old_node->add_child(wpath);
        }
    }
    node->add_child(paths[paths.size() - 1].toStdWString(), text_edit->toPlainText().toStdWString());
}
void store_info(QCheckBox* box, XMLNode* root){
    auto paths = box->objectName().split('_');
    XMLNode* node = root;
    for (int i = 0; i < paths.size() - 1; i++){
        auto old_node = node;
        std::wstring wpath = paths[i].toStdWString();
        node = old_node->get_child(wpath);
        if (!node){
            node = old_node->add_child(wpath);
        }
    }
    node->add_child(paths[paths.size() - 1].toStdWString(), std::to_wstring(box->isTristate()));
}
void store_info(QComboBox* box, XMLNode* root){
    auto paths = box->objectName().split('_');
    XMLNode* node = root;
    for (int i = 0; i < paths.size() - 1; i++){
        auto old_node = node;
        std::wstring wpath = paths[i].toStdWString();
        node = old_node->get_child(wpath);
        if (!node){
            node = old_node->add_child(wpath);
        }
    }
    node->add_child(paths[paths.size() - 1].toStdWString(), box->currentText().toStdWString());
}
SettingsWindow::SettingsWindow(std::wstring path_fl, std::wstring path_file, QWidget *parent) :
        QWidget(parent),
        path_fl(path_fl),
        path_file(path_file),
        ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    std::wifstream file(path_fl);
    file.imbue(std::locale("ru_RU.UTF-8"));
    if (path_fl.empty()){
        QMessageBox message_box;
        message_box.critical(0, "Не возможно открыть файл", "Вы не ввели путь");
        message_box.setFixedSize(300, 200);
        return;
    }
    if (!std::filesystem::exists(path_fl)){
        QMessageBox message_box;
        message_box.critical(0, "Не возможно открыть файл", "Файл не существует");
        message_box.setFixedSize(300, 200);
        return;
    }
    if (!file.is_open()){
        QMessageBox message_box;
        message_box.critical(0, "Не возможно открыть файл", "Недостаточно прав");
        message_box.setFixedSize(300, 200);
        return;
    }
    try {
        XML xml(file);
        auto node_tabs = xml.get_child(L"tabs");
        for (auto& i : *node_tabs){
            add_tab(i.first, *i.second);
        }
        auto node_elems = xml.get_child(L"elems");
        for (auto& i : *node_elems){
            add_elem(i.first, *i.second);
        }
        QMetaObject::connectSlotsByName(this);
    }
    catch (std::exception& er){
        QMessageBox message_box;
        message_box.critical(0, "Ошибка", er.what());
        message_box.setFixedSize(300, 200);
        return;
    }
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::add_elem(const std::wstring &name, XMLNode &node){
    std::wstring tab = node.get_child(L"tab")->get_value();
    auto found_tab = find_tab(tab);
    if (!found_tab){
        throw UnknownTab();
    }
    std::wstring type = node.get_child(L"type")->get_value();
    if (type == L"Integer"){
        spin_boxes.push_back(found_tab->add_integer(name, node));
    }
    else if (type == L"Double"){
        double_spin_boxes.push_back(found_tab->add_double(name, node));
    }
    else if (type == L"String"){
        text_edits.push_back(found_tab->add_string(name, node));
    }
    else if (type == L"Boolean"){
        check_boxes.push_back(found_tab->add_boolean(name, node));
    }
    else if (type == L"Space"){
        found_tab->add_space(name, node);
    }
    else if (type == L"PathFolder"){
        text_edits.push_back(found_tab->add_path_folder(name, node));
    }
    else if (type == L"PathFile"){
        text_edits.push_back(found_tab->add_path_file(name, node));
    }
    else if (type == L"Enum"){
        combo_boxes.push_back(found_tab->add_enum(name, node));
    }
    else throw UnknownTypeSettings();
}

MyTab* SettingsWindow::add_tab(std::wstring name, XMLNode& node)
{
    auto it = (tabs.emplace(name, MyTab(name)));
    std::wstring title = node.get_child(L"title")->get_value();
    ui->tabWidget->addTab(it.first->second.get_tab(), QString::fromStdWString(title));
    return &(it.first->second);
}

bool operator==(const std::pair<std::wstring, MyTab>& elem, const std::wstring& key){
    return elem.first == key;
}
MyTab* SettingsWindow::find_tab(const std::wstring& key)
{
    auto it = std::find(tabs.begin(), tabs.end(), key);
    if (it != tabs.end()){
        return &(it->second);
    }
    else return nullptr;
}

void SettingsWindow::on_load_button_clicked()
{
    if (path_file.empty()){
        QMessageBox message_box;
        message_box.critical(0, "Не возможно открыть файл", "Вы не ввели путь");
        message_box.setFixedSize(300, 200);
        return;
    }
    if (!std::filesystem::exists(path_file)){
        QMessageBox message_box;
        message_box.critical(0, "Не возможно открыть файл", "Файл не существует");
        message_box.setFixedSize(300, 200);
        return;
    }
    std::wifstream file(path_file);
    if (!file.is_open()){
        QMessageBox message_box;
        message_box.critical(0, "Не возможно открыть файл", "Недостаточно прав");
        message_box.setFixedSize(300, 200);
        return;
    }
    file.imbue(std::locale("ru_RU.UTF-8"));
    XML xml(file);
    auto node = xml.get_root();
    for (auto& i : check_boxes){
        load_info(i, node);
    }
    for (auto& i : spin_boxes){
        load_info(i, node);
    }
    for (auto& i : double_spin_boxes){
        load_info(i, node);
    }
    for (auto& i : text_edits){
        load_info(i, node);
    }
    for (auto& i : combo_boxes){
        load_info(i, node);
    }
}

void SettingsWindow::on_save_button_clicked()
{
    if (path_file.empty()){
        QMessageBox message_box;
        message_box.critical(0, "Не возможно открыть файл", "Вы не ввели путь");
        message_box.setFixedSize(300, 200);
        return;
    }
    std::wofstream file(path_file);
    if (!file.is_open()){
        QMessageBox message_box;
        message_box.critical(0, "Не возможно открыть или создать файл", "Недостаточно прав");
        message_box.setFixedSize(300, 200);
        return;
    }
    file.imbue(std::locale("ru_RU.UTF-8"));
    XML xml;
    auto node = xml.get_root();
    for (auto& i : check_boxes){
        store_info(i, node);
    }
    for (auto& i : spin_boxes){
        store_info(i, node);
    }
    for (auto& i : double_spin_boxes){
        store_info(i, node);
    }
    for (auto& i : text_edits){
        store_info(i, node);
    }
    for (auto& i : combo_boxes){
        store_info(i, node);
    }
    xml.save_to_file(file);
    emit resaved_file();
}

