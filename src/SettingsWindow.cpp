#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"
#include "xml.h"
#include <QScrollArea>
#include <QFormLayout>
#include <filesystem>
#include <fstream>
#include <QMessageBox>

XMLNode* find_node_object(QObject* object, std::map<QString, XML>& xmls){
    auto file_and_tag = object->objectName().split('/');
    auto paths = file_and_tag[1].split('_');
    XML& xml = xmls[file_and_tag[0]];
    auto node = xml.get_root();
    for (auto& path : paths){
        node = node->get_child(path.toStdWString());
    }
    return node;
}
XMLNode* add_node_object(QObject* object, std::map<QString, XML>& xmls){
    auto file_and_tag = object->objectName().split('/');
    auto paths = file_and_tag[1].split('_');
    XML& xml = xmls[file_and_tag[0]];
    auto node = xml.get_root();
    for (auto& path : paths){
        node = node->add_child(path.toStdWString());
    }
    return node;
}
void load_info(QSpinBox* box, std::map<QString,XML>& xmls){
    auto node = find_node_object(box, xmls);
    if (node) box->setValue(std::stoi(node->get_value()));
}
void load_info(QDoubleSpinBox* box, std::map<QString,XML>& xmls){
    auto node = find_node_object(box, xmls);
    if (node) box->setValue(std::stod(node->get_value()));
}
void load_info(QCheckBox* box, std::map<QString,XML>& xmls){
    auto node = find_node_object(box, xmls);
    if (node) box->setTristate(std::stoi(node->get_value()));
}
void load_info(QTextEdit* box, std::map<QString,XML>& xmls){
    auto node = find_node_object(box, xmls);
    if (node) box->setText(QString::fromStdWString(node->get_value()));
}
void load_info(QComboBox* box, std::map<QString,XML>& xmls){
    auto node = find_node_object(box, xmls);
    if (node) box->setCurrentText(QString::fromStdWString(node->get_value()));
}
void store_info(QSpinBox* box, std::map<QString,XML>& xmls){
    auto node = add_node_object(box, xmls);
    node->change_type(XMLNodeType::VALUE);
    node->set_value(box->text().toStdWString());
}
void store_info(QDoubleSpinBox* box, std::map<QString,XML>& xmls){
    auto node = add_node_object(box, xmls);
    node->change_type(XMLNodeType::VALUE);
    node->set_value(box->text().toStdWString());
}
void store_info(QTextEdit* text_edit, std::map<QString,XML>& xmls){
    auto node = add_node_object(text_edit, xmls);
    node->change_type(XMLNodeType::VALUE);
    node->set_value(text_edit->toPlainText().toStdWString());
}
void store_info(QCheckBox* box, std::map<QString,XML>& xmls){
    auto node = add_node_object(box, xmls);
    node->change_type(XMLNodeType::VALUE);
    node->set_value(std::to_wstring(box->isTristate()));
}
void store_info(QComboBox* box, std::map<QString,XML>& xmls){
    auto node = add_node_object(box, xmls);
    node->change_type(XMLNodeType::VALUE);
    node->set_value(box->currentText().toStdWString());
}
SettingsWindow::SettingsWindow(std::wstring dir, std::wstring name_fl, QString name, QWidget *parent) :
        QMainWindow(parent),
        _name_fl(name_fl),
        _dir(dir),
        ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    ui->label_name->setText(name);
    QObject::connect(ui->toolButton_minimize, &QToolButton::clicked, this, &SettingsWindow::showMinimized);
    QObject::connect(ui->toolButton_exit, &QToolButton::clicked, this, &SettingsWindow::close);
    std::wstring path_fl;
    if (!dir.empty()) {
        path_fl+= dir;
        path_fl+=  L'/';
    }
    path_fl+= name_fl;
    std::wifstream file(path_fl);
    file.imbue(std::locale("ru_RU.UTF-8"));
    if (!std::filesystem::exists(path_fl)){
        QMessageBox message_box;
        message_box.critical(0, "Ошибка настроек", "Файл форм(.fl) не найден.");
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
        auto node_files = xml.get_child(L"files");
        for (auto& file : *node_files){
            _files.push_back(QString::fromStdWString(file.second->get_value()));
        }
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
        _spin_boxes.push_back(found_tab->add_integer(name, node));
    }
    else if (type == L"Double"){
        _double_spin_boxes.push_back(found_tab->add_double(name, node));
    }
    else if (type == L"String"){
        _text_edits.push_back(found_tab->add_string(name, node));
    }
    else if (type == L"Boolean"){
        _check_boxes.push_back(found_tab->add_boolean(name, node));
    }
    else if (type == L"Space"){
        found_tab->add_space(name, node);
    }
    else if (type == L"PathFolder"){
        _text_edits.push_back(found_tab->add_path_folder(name, node));
    }
    else if (type == L"PathFile"){
        _text_edits.push_back(found_tab->add_path_file(name, node));
    }
    else if (type == L"Enum"){
        _combo_boxes.push_back(found_tab->add_enum(name, node));
    }
    else throw UnknownTypeSettings();
}

MyTab* SettingsWindow::add_tab(std::wstring name, XMLNode& node)
{
    auto it = (_tabs.emplace(name, MyTab(name)));
    std::wstring title = node.get_child(L"title")->get_value();
    QWidget* tab = it.first->second.get_tab();
    auto scroll = it.first->second.get_scroll_area();
    auto scrollContent = it.first->second.get_scroll_area_contents();
    ui->tabWidget->addTab(tab, QString::fromStdWString(title));
    QString styleSheet = ui->tabWidget->styleSheet();
    tab->setStyleSheet(styleSheet);
    scroll->setStyleSheet(styleSheet);
    scrollContent->setStyleSheet(styleSheet);
    return &(it.first->second);
}

bool operator==(const std::pair<std::wstring, MyTab>& elem, const std::wstring& key){
    return elem.first == key;
}
MyTab* SettingsWindow::find_tab(const std::wstring& key)
{
    auto it = std::find(_tabs.begin(), _tabs.end(), key);
    if (it != _tabs.end()){
        return &(it->second);
    }
    else return nullptr;
}

void SettingsWindow::on_load_button_clicked()
{
    std::map<QString, XML> xmls;
    for (auto & file : _files){
        std::wstring full_path;
        if (!_dir.empty()) {
            full_path+= _dir;
            full_path+=  L'/';
        }
        full_path+= file.toStdWString();
        std::wifstream wfile (full_path);
        wfile.imbue(std::locale("ru_RU.UTF-8"));
        xmls[file] = XML(wfile);
    }
    for (auto& i : _check_boxes){
        load_info(i, xmls);
    }
    for (auto& i : _spin_boxes){
        load_info(i, xmls);
    }
    for (auto& i : _double_spin_boxes){
        load_info(i, xmls);
    }
    for (auto& i : _text_edits){
        load_info(i, xmls);
    }
    for (auto& i : _combo_boxes){
        load_info(i, xmls);
    }
}

void SettingsWindow::on_save_button_clicked()
{
    std::map<QString, XML> xmls;
    for (auto& i : _check_boxes){
        store_info(i, xmls);
    }
    for (auto& i : _spin_boxes){
        store_info(i, xmls);
    }
    for (auto& i : _double_spin_boxes){
        store_info(i, xmls);
    }
    for (auto& i : _text_edits){
        store_info(i, xmls);
    }
    for (auto& i : _combo_boxes){
        store_info(i, xmls);
    }
    for (auto& xml : xmls){
        std::wstring full_path;
        if (!_dir.empty()) {
            full_path+= _dir;
            full_path+=  L'/';
        }
        full_path+= xml.first.toStdWString();
        std::wofstream wfile (full_path);
        wfile.imbue(std::locale("ru_RU.UTF-8"));
        xml.second.save_to_file(wfile);
    }
    emit resaved_file();
}

